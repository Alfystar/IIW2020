//
// Created by filippob on 11/09/20.
//

#include "resource.h"

using namespace CES;

Resource::Resource(string &path, float qValue) : Resource::Resource(path, (string &) "*", qValue) {
    //todo: capire se va aggiunto altro
}

Resource::Resource(string &p, string &format, float qValue) {


    pthread_rwlock_rdlock(rwlock); // LOCK SHREDDER

    string scale = to_string((int) round(qValue * 100));

    string tmp;
    if (format == "*") {
        tmp = p.substr(p.rfind('/') + 1, p.rfind('.') - 1) + "_" + scale + "_." +
              p.substr(p.rfind('.') + 1, p.length()); // estensione

    } else {
        tmp = p.substr(p.rfind('/') + 1, p.rfind('.') - 1) + "_" + scale + "_." + format;

    }
    path = tmp;

    openMutex.lock();

    if ((fd = open(path.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) { // provo a creare il file
        if (errno != EEXIST) perror("[Resource]Error occurred at 1st open: ");

        cout << path << "[Resource] file already exists";
        fd = open(path.c_str(), 0, S_IRUSR); // accedo al file in sola lettura
        openMutex.unlock();
        flock(fd, LOCK_SH); //in questo modo rimango in attesa che venga elaborato prima il file

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

    string command = "misc/magick convert " + file + " -resize " + scale + "% " + this->path;
    // magick permette di fare la conversione e il resize in un unico comando,
    // visto che la stringa verrà interpretata dal programma

    system(command.c_str()); // esecuzione magick

    struct stat buf{};
    stat(file.c_str(), &buf);
    int fileSize = buf.st_size;

    write(sizePipe[writeEndPipe], &fileSize, sizeof(int));

}

