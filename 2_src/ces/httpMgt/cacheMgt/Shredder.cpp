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

    initCache();
    initSizePipe();

    //todo: verificare una volta nel costruttore la dimensione della cache, visto che potrebbero già esserci dei file

    tShr = new thread(threadShr, this);
}

[[noreturn]] void Shredder::threadShr(Shredder *s) {

    cout << "[Shredder::threadShr()] Shredder started\n";

    for (;;) {


        pthread_rwlock_wrlock(rwlock); //LOCK

        cout << "Verifying size of cache\n";


        s->fillImgsVect(s->cache_path); // obtain a vector with all the images

        if (s->sizeOfCache() >
            FILE_SIZE_LIMIT) { //todo.txt: implementare pipe da controllare con la dimensione dei nuovi file creati
            s->reduceCacheUsage();
        }

        pthread_rwlock_unlock(rwlock); //UNLOCK

        s->emptyCache();
        //forse non si può comunque fare, visto che dovremmo vedere l'ultimo accesso

        sleep(SLEEP_TIME);
    }
}


void Shredder::fillImgsVect(string &path) { //obtain a vector with all the files from the filesystem

    for (auto &p: fs::recursive_directory_iterator(path)) {
        if (!fs::is_directory(p.path()) && (p.path().string().find("/.") == string::npos)) {
            auto *i = new ImgData(std::string(p.path().string()));
            imgsVect.push_back(*i);
        }
    }
}

uint_fast64_t Shredder::sizeOfCache() {
    uint_fast64_t size = 0;
    for (auto &img : imgsVect) {
        size += img.buf->st_size;
    }
    cout << "[Shredder::sizeOfCache()] size of cache: " + to_string(size) + "\n";
    return size;
}

void Shredder::reduceCacheUsage() {
    sort(this->imgsVect.begin(), this->imgsVect.end()); //sort by last access time
    // todo.txt: verificare se l'ordine è conveniente per fare la pop, in caso cambiare il verso in operator di IMGDATA
    for (ulong i = 0; i < (imgsVect.size() / 2); i++) {
        imgsVect.back().removeFile();
        imgsVect.pop_back();
    }
    // la restante parte viene deallocata dopo il rilascio del mutex, e prima dello sleep
}

void Shredder::emptyCache() {
    while (!imgsVect.empty()) {
        imgsVect.pop_back();
    }
}

void Shredder::initCache() {

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
}