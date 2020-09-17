//
// Created by filippob on 12/09/20.
//


#include "Shredder.h"

using namespace CES;

/// ImgData, used for sorting and working on filesystem

ImgData::ImgData(const string &p) { //otteniamo un oggetto con i dati per il sort
    path = p;
    buf = (struct stat *) (malloc(sizeof(struct stat)));

    if (stat(p.c_str(), buf)) {
        perror("File Stats error: ");
        path = nullptr;
        buf = nullptr;
    }

}

ImgData::~ImgData() {
    path = nullptr;
    free(buf);
}

void ImgData::removeFile() {
    if (remove(path.c_str())) {
        string tmp_str = "Failed to remove " + path + " , because: ";
        perror(tmp_str.c_str());
    }
    this->~ImgData();
}



/// Shredder functions

Shredder *Shredder::instance = nullptr;

Shredder *Shredder::getInstance() { //not thread-safe
    cout << "[Shredder::getInstance()] Got Instance\n";
    if (!instance)
        instance = new Shredder();
    return instance;
}

Shredder::Shredder() {

    if (initShredderLock()) {
        cout << "[Shredder()] Fallita inizializzazione della sincronia";
        exit(-1);
    }

    initSizePipe();
    initCache();

    //todo: verificare una volta nel costruttore la dimensione della cache, visto che potrebbero già esserci dei file

    tShr = new thread(threadShr, this);
}

[[noreturn]] void Shredder::threadShr(Shredder *s) {

    cout << "[Shredder::threadShr()] Shredder started\n";

    for (;;) {

        s->waitUntilFullCache();

        pthread_rwlock_wrlock(rwlock); //LOCK

        cout << "Verifying size of cache\n";

        s->fillImgVect(s->cache_path); // obtain a vector with all the images
        s->reduceCacheUsage();

        pthread_rwlock_unlock(rwlock); //UNLOCK

        s->emptyImgVect();
    }
}


void Shredder::fillImgVect(string &path) { //obtain a vector with all the files from the filesystem

    for (auto &p: fs::recursive_directory_iterator(path)) {
        if (!fs::is_directory(p.path()) && (p.path().string().find("/.") == string::npos)) {
            auto *i = new ImgData(std::string(p.path().string()));
            imgVect.push_back(*i);
        }
    }
}

uint_fast64_t Shredder::sizeOfCache() {
    uint_fast64_t size = 0;
    for (auto &img : imgVect) {
        size += img.buf->st_size;
    }
    cout << "[Shredder::sizeOfCache()] size of cache: " + to_string(size) + "\n";
    return size;
}

void Shredder::reduceCacheUsage() {
    sort(this->imgVect.begin(), this->imgVect.end()); //sort by last access time
    // todo.txt: verificare se l'ordine è conveniente per fare la pop, in caso cambiare il verso in operator di IMGDATA
    for (ulong i = 0; i < (imgVect.size() / 2); i++) {
        imgVect.back().removeFile();
        imgVect.pop_back();
    }
    // la restante parte viene deallocata dopo il rilascio del mutex, e prima dello sleep
}

void Shredder::emptyImgVect() {
    while (!imgVect.empty()) {
        imgVect.pop_back();
    }
}

int Shredder::initSizePipe() {
    if (pipe2(sizePipe, O_DIRECT | O_NONBLOCK)) {
        perror("[initSizePipe]: ");
        return -1;
    }
    return 0;
}

void Shredder::initCache() { // init cache and cache_size (if some file are already there)

    string absPath = string(get_current_dir_name());
    if (absPath.substr(absPath.length() - 6, absPath.length()) != "webRsc") {
        perror("[initCache] Critical error: pwd is NOT webRsc");
    }
    if (!fs::exists(cache_path)) {
        fs::create_directories(cache_path);
    }
    string prefix = "web/img";

    for (auto &p: fs::directory_iterator(prefix)) {

        if (!fs::is_directory(p.path()) && (p.path().string().find("/.") == string::npos)) {

            string newFold = p.path().string().substr(prefix.length(), p.path().string().length());
            newFold = newFold.substr(1, newFold.find('.') - 1);

            fs::create_directory(cache_path + '/' + newFold);
        }
    }
    // a prescindere, inizializzo la dimensione della cache (sopra si crea la cartella, ma se già c'e non si fa nulla)

    this->fillImgVect(cache_path);
    cacheSize = this->sizeOfCache();
    this->emptyImgVect();
}

void Shredder::updateSizeCache(int fSize) {
    write(sizePipe[writeEndPipe], &fSize, sizeof(int));
}

void Shredder::waitUntilFullCache() {

    timespec t{5, 0};
    sigset_t sigmask;
    sigfillset(&sigmask);
    int ready;
    int size;

    while (true) {
        ready = ppoll(&pollfd, 1, &t, &sigmask);
        switch (ready) {
            case -1:
                perror("[Shredder::waitUntilFullCache]: ");
                exit(EX_OSERR);

            case 0:
#ifdef DEBUG_LOG
                Log::db << "Shredder::waitUntilFullCache() ppoll time-out 1 s\n";
#endif
                break;

            default: //siccome guardiamo solo un file descriptor, sarà sicuramente quello

                bool end = false;
                do {
                    if (read(sizePipe[readEndPipe], &size, sizeof(int)) == -1) {
                        switch (errno) {
                            case EAGAIN:
                                // la lettura vorrebbe bloccare, ma noi l'abbiamo segnata non bloccante
                                // Quindi la pipe è stata svuotata
                                end = true; //termino il while perchè ho lettot tutto
                                break;
                            default:
                                perror("Queue::popWaitingCon pipe write error:");
                                sleep(1);
                                exit(-1);
                        }
                    }
                    cacheSize += size;
                } while (!end);
                break;
        }
        if (cacheSize > FILE_SIZE_LIMIT) break;
    }
}
