//
// Created by filippob on 11/09/20.
//

#include "Resource.h"

using namespace CES;

Resource::Resource (string &path, float qValue) : Resource::Resource(path, (string &) "*", qValue){
}

Resource::Resource (string &p, string &format, float qValue){
    if (qValue == 1.0 && format == "*"){
        this->path = p;
        //fd is not initialised, since is not required synchrony
        return;
    }
    if (format == "jpeg") format = "jpg";

    pthread_rwlock_rdlock(rwlock); // LOCK SHREDDER

    string scale = to_string((int) round(qValue * 100));
    std::size_t st;
    std::size_t end;
    st = p.rfind('/') + 1;
    end = p.rfind('.');
    string tmp, nameOriginal, extension;
    if (format == "*"){
        nameOriginal = p.substr(st, end - st);
        end += 1;
        extension = p.substr(end, p.length() - end);
        tmp = nameOriginal + "_" + scale + "_." + extension;
    }
    else{
        nameOriginal = p.substr(st, end - st);
        tmp = nameOriginal + "_" + scale + "_." + format;
    }
    // partiamo da 2 perche il path p = "./web....", quindi i primi due sono sicuramente quei simboli
    string relSubFold = p.substr(1, (p.rfind('/')));
    string subFolder = string(CACHE_PATH) + relSubFold + nameOriginal;
    path = subFolder + "/" + tmp;

    fs::create_directories(subFolder);

    openMutex.lock();

    if ((fd = open(path.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1){ // Provo a creare il file
        if (errno != EEXIST){   // Se l'errore non è che il file già esiste è anomalo
            perror("[Resource::Resource]Error occurred at 1st open: ");
            cerr << path << endl;
            cerr << "format: " << format << endl;
            cerr << "qValue: " << qValue << endl;
            path = p;
            openMutex.unlock();
            return;
        }
        #ifdef DEBUG_LOG
        Log::db << "[Resource::Resource] file already exists:\n\t" << path << "\n";
        #endif
        fd = open(path.c_str(), 0, S_IRUSR); // accedo al file in sola lettura
        openMutex.unlock();
        flock(fd, LOCK_SH); //in questo modo rimango in attesa che venga elaborato prima il file
        return;
    }

    flock(fd, LOCK_EX);
    openMutex.unlock();
    this->elaborateFile(p, scale);
    flock(fd, LOCK_SH);
}

Resource::~Resource (){
    //destroy the resource, then close the fd and delete the file
    flock(fd, LOCK_UN);
    close(fd);
    pthread_rwlock_unlock(rwlock); //UNLOCK SHREDDER
}

string &Resource::getPath (){
    return path;
}

void Resource::elaborateFile (string &file, string &scale){
    string format = file.substr(file.rfind('.') + 1, file.length());

    if (format == "webp"){
        string tmpFile = "dwebp " + file + " -o " + this->path; //for decoding webp images to png; IT DOESN'T RESIZE
        executeCommand(tmpFile);
        file = this->path;
    }

    string command = "./misc/magick convert " + file + " -resize " + scale + "% " + this->path;
    executeCommand(command);

    struct stat buf{};
    stat(path.c_str(), &buf);
    int fileSize = buf.st_size;

    Shredder::getInstance()->updateSizeCache(fileSize);
}

void Resource::executeCommand (string &command){
    #ifdef DEBUG_LOG
    Log::db << "[Resource::elaborateFile] execute bash command:\n";
    Log::db << "@bash>> " << command << endl;
    #endif

    command += " 2>&1";
    FILE *outCommand = popen(command.c_str(), "r"); // esecuzione magick
    if (!outCommand){
        // warning: if memory allocator fails, errno is not set
        perror("[Resource::elaborateFile] popen() failed due to ");
    }

    #ifdef DEBUG_LOG
    char outBuff[1024];
    int sizeFRead = fread_unlocked(outBuff, 1, sizeof(outBuff) - 1, outCommand);
    outBuff[sizeFRead] = '\0';
    Log::db << outBuff << '\n';
    #endif

    if (pclose(outCommand) == -1){
        perror("[Resource::elaborateFile] pclose() failed due to ");
    }
}