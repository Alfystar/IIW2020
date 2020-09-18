//
// Created by filippob on 11/09/20.
//

#include "Resource.h"

using namespace CES;

Resource::Resource(string &path, float qValue) : Resource::Resource(path, (string &) "*", qValue) {
}

Resource::Resource(string &p, string &format, float qValue) {

    if (qValue == 1.0 && format == "*") {
        this->path = p;
        //fd is not initialised, since is not required synchrony
        return;
    }

    pthread_rwlock_rdlock(rwlock); // LOCK SHREDDER

    string scale = to_string((int) round(qValue * 100));

    std::size_t st;
    std::size_t end;
    st = p.rfind('/') + 1;
    end = p.rfind('.');

    string tmp, nameOriginal, extension;
    if (format == "*") {
        nameOriginal = p.substr(st, end - st);
        end += 1;
        extension = p.substr(end, p.length() - end);
        tmp = nameOriginal + "_" + scale + "_." + extension;
    } else {
        nameOriginal = p.substr(st, end - st);
        tmp = nameOriginal + "_" + scale + "_." + format;
    }

    string subFolder = string(CACHE_PATH) + "/" + nameOriginal;
    path = subFolder + "/" + tmp;

    fs::create_directories(subFolder);
    //todo: supponiamo che almeno i file siano chiamati in modo diverso? è il problema che si pone se ho cartelle diverse!

    openMutex.lock();

    if ((fd = open(path.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) { // provo a creare il file
        if (errno != EEXIST) {
            perror("[Resource::Resource]Error occurred at 1st open: ");
            cerr << path << endl;
            //todo: ritornare un errore o il file originale o vedere [EDIT: in teoria non dovrebbe più entrare qui]
            path = p;
            openMutex.unlock();
            return;
        }
        cout << path << "[Resource::Resource] file already exists\n";
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

Resource::~Resource() {
//destroy the resource, then close the fd and delete the file
    flock(fd, LOCK_UN);
    close(fd);

    pthread_rwlock_unlock(rwlock); //UNLOCK SHREDDER

}


string &Resource::getPath() {
    return path;
}

void Resource::elaborateFile(string &file, string &scale) {

    string command = "./misc/magick convert " + file + " -resize " + scale + "% " + this->path;
    // magick permette di fare la conversione e il resize in un unico comando,
    // visto che la stringa verrà interpretata dal programma
    cerr << "Resource::elaborateFile esegue system con:\n";
    cerr << command << endl;
    system(command.c_str()); // esecuzione magick

    struct stat buf{};
    stat(file.c_str(), &buf);
    int fileSize = buf.st_size;

    Shredder::getInstance()->updateSizeCache(fileSize);
}

