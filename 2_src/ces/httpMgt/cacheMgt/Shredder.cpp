//
// Created by filippob on 12/09/20.
//


#include "Shredder.h"

using namespace CES;

/// ImgData, used for sorting and working on filesystem

ImgData::ImgData(const string &p) { //otteniamo un oggetto con i dati per il sort
    path = p;
    statFile = (struct stat *) (malloc(sizeof(struct stat)));

    if (stat(p.c_str(), statFile)) {
        perror("File Stats error: ");
        path = nullptr;
        statFile = nullptr;
    }

}

ImgData::~ImgData() {
    if (!statFile)
        free(statFile);
}

int ImgData::removeFile() {
    if (remove(path.c_str())) {
        string tmp_str = "Failed to remove " + path + " , because: ";
        perror(tmp_str.c_str());
    }
    string dir = path.substr(0, path.rfind('/') - 1); //todo: verificare correttezza
    if (fs::is_empty(dir)) {
        fs::remove(dir);
    }
    return statFile->st_size;
}



/// Shredder functions

Shredder *Shredder::instance = nullptr;

Shredder *Shredder::getInstance() { //not thread-safe
//    cout << "[Shredder::getInstance()] Got Instance\n";
    if (!instance)
        instance = new Shredder();
    return instance;
}

Shredder::Shredder() {

    if (initShredderLock()) {
        cout << "[Shredder()] Fallita inizializzazione della sincronia";
        exit(EX_OSERR);
    }

    initSizePipe();
    initCache();

    tShr = new thread(threadShr, this);
}

[[noreturn]] void Shredder::threadShr(Shredder *s) {

    cout << "[Shredder::threadShr] Shredder started\n";
    if (s->cacheSize > FILE_SIZE_LIMIT) {
        s->freeSpace();
    }
    for (;;) {
        s->waitUntilFullCache();    // aspettare la pool
        //todo: capire perchè cacheSize non si aggiorna
        s->elaboratePipe();         // svuoto la pipe
        cout << "Shredder::threadShr Verifying size of cache\n";
        if (s->cacheSize < FILE_SIZE_LIMIT) {
            continue;
        }
        s->freeSpace();
    }
}


void Shredder::fillImgVect(string &path) { //obtain a vector with all the files from a directory

    for (auto &p: fs::recursive_directory_iterator(path)) {
        if (!fs::is_directory(p.path()) && (p.path().string().find("/.") == string::npos)) { // for .gitignore
            auto *i = new ImgData(std::string(p.path().string()));
            imgVect.push_back(*i);
        }
    }
}

uint_fast64_t Shredder::sizeOfCache() {
    uint_fast64_t size = 0;
    for (auto &img : imgVect) {
        size += img.statFile->st_size;
    }
    cout << "[Shredder::sizeOfCache()] size of cache: " + to_string(size) + "\n";
    return size;
}

void Shredder::reduceCacheUsage() {
    sort(this->imgVect.begin(), this->imgVect.end()); //sort by last access time

    // todo: cancellare finito il debug
    for (auto a : this->imgVect) {  // printa la lista sort-ata
        std::cout << a.path << " time:" << a.statFile->st_atim.tv_sec << "\n";
    }
    while (cacheSize > FILE_SIZE_LIMIT / 2) {
        cacheSize -= imgVect.back().removeFile();
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
    pollfd = {sizePipe[readEndPipe], POLLIN, 0};
    return 0;
}

void Shredder::initCache() { // init cache and cache_size (if some file are already there)

    string absPath = string(get_current_dir_name());
    if (absPath.substr(absPath.length() - 6, absPath.length()) != "webRsc") {
        perror("[initCache] Critical error: pwd is NOT webRsc");
        exit(EX_CONFIG);
    }
    if (!fs::exists(cache_path)) {
        fs::create_directories(cache_path);
    }
    //init cacheSize with already existing files
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
                continue;
                break;
            default: //siccome guardiamo solo un file descriptor, c'è qualcosa da leggere
                return;
        }
    }
}

void Shredder::elaboratePipe() {
    int size;
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
                    perror("Shredder::waitUntilFullCache pipe write error:");
                    sleep(1);
                    exit(-1);
            }
        }
        cacheSize += size;
    } while (!end);
}

void Shredder::freeSpace() {
    pthread_rwlock_wrlock(rwlock); //LOCK

    fillImgVect(cache_path); // obtain a vector with all the images
    reduceCacheUsage();

    pthread_rwlock_unlock(rwlock); //UNLOCK

    emptyImgVect();
}
