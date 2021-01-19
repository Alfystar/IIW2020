//
// Created by filippob on 12/09/20.
//

#include "Shredder.h"

using namespace CES;

/// ImgData, used for sorting and working on filesystem

Shredder::ImgData::ImgData (const string &p){ //otteniamo un oggetto con i dati per il sort
    path = p;
    statFile = (struct stat *) (malloc(sizeof(struct stat)));

    if (stat(p.c_str(), statFile)){
        perror("File Stats error: ");
        path = nullptr;
        statFile = nullptr;
    }
}

Shredder::ImgData::~ImgData (){
    if (!statFile)
        free(statFile);
}

int Shredder::ImgData::removeFile (){
    if (remove(path.c_str())){
        string tmp_str = "Failed to remove " + path + " , because: ";
        perror(tmp_str.c_str());
    }
    string dir = path.substr(0, path.rfind('/'));
    if (fs::is_empty(dir)){
        fs::remove(dir);
    }
    return statFile->st_size;
}



/// Shredder functions

Shredder *Shredder::instance = nullptr;

Shredder *Shredder::getInstance (){ //not thread-safe
    if (!instance)
        instance = new Shredder();
    return instance;
}

Shredder::Shredder (){
    cout << "[Shredder::Shredder] Initialize Shredder\n";
    if (initShredderLock()){
        cerr << "[Shredder::Shredder] Fallita inizializzazione della sincronia";
        exit(EX_OSERR);
    }
    initSizePipe();
    initCache();
    tShr = new thread(threadShr, this);
}

[[noreturn]] void Shredder::threadShr (Shredder *s){
    pthread_setname_np(pthread_self(), "Shredder");
    #ifdef DEBUG_LOG
    Log::db << "[Shredder::threadShr] Shredder started\n";
    #endif
    if (s->cacheSize > FILE_SIZE_LIMIT){
        s->freeSpace();
    }
    for (;;){
        s->waitUntilFullCache();    // aspettare la poll
        s->elaboratePipe();         // svuoto la pipe
        if (s->cacheSize < FILE_SIZE_LIMIT) {
            continue;
        }
        s->freeSpace();
    }
}

void Shredder::fillImgVect (string &path){ //obtain a vector with all the files from a directory
    emptyImgVect();
    for (auto &p: fs::recursive_directory_iterator(path)){
        if (!fs::is_directory(p.path()) && (p.path().string().find("/.") == string::npos)){ // for .gitignore
            auto *i = new ImgData(std::string(p.path().string()));
            imgVect.push_back(*i);
        }
    }
}

uint_fast64_t Shredder::sizeOfCache (){
    uint_fast64_t size = 0;
    countFile = 0;
    for (auto &img : imgVect){
        size += img.statFile->st_size;
        countFile++;
    }
    #ifdef DEBUG_LOG
    Log::db << "[Shredder::sizeOfCache] Size of cache: " + sizeFormatted(size) + ";\t N° Files: " +
               to_string(countFile) + '\n';
    #endif
    return size;
}

void Shredder::reduceCacheUsage (){
    sort(this->imgVect.begin(), this->imgVect.end()); //sort by last access time

    #ifdef DEBUG_LOG
    Log::db << "[Shredder::reduceCacheUsage] Actual size of cache: " + sizeFormatted(cacheSize) + ";\t N° Files: " +
               to_string(countFile) + '\n';

    for (auto a : this->imgVect){  // printa la lista sort-ata
        struct tm *nowtm;
        char tmbuf[64];
        nowtm = localtime(&a.statFile->st_atim.tv_sec);
        strftime(tmbuf, sizeof(tmbuf), "%F %T", nowtm);
        Log::db << " time: " << tmbuf << "\t" << a.path << "\n";
    }
    #endif

    while (cacheSize > FILE_SIZE_LIMIT / 2 && imgVect.size() > 1){
        int size = imgVect.back().removeFile();
        cacheSize -= size;
        imgVect.pop_back();
    }
    countFile = imgVect.size();

    #ifdef DEBUG_LOG
    Log::db << "\n[Shredder::reduceCacheUsage] New size of cache: " + sizeFormatted(cacheSize) + ";\t N° Files: " +
               to_string(countFile) + '\n';

    for (auto a : this->imgVect){  // printa la lista sort-ata
        struct tm *nowtm;
        char tmbuf[64];
        nowtm = localtime(&a.statFile->st_atim.tv_sec);
        strftime(tmbuf, sizeof(tmbuf), "%F %T", nowtm);
        Log::db << " time: " << tmbuf << "\t" << a.path << "\n";
    }
    #endif
}

inline void Shredder::emptyImgVect (){
    imgVect.clear();
}

int Shredder::initSizePipe (){
    if (pipe2(sizePipe, O_DIRECT)){
        perror("[initSizePipe]: ");
        return -1;
    }
    // Rendo NON BLOCCANTE solo l'estremo in lettura
    if (fcntl(sizePipe[readEndPipe], F_SETFL, O_NONBLOCK) < 0){
        perror("[Queue::Queue] Error make waitPipe[readEnd] O_NONBLOCK:");
        exit(EX_IOERR);
    }

    pollFd = {sizePipe[readEndPipe], POLLIN, 0};
    return 0;
}

void Shredder::initCache (){ // init cache and cache_size (if some file are already there)
    if (!fs::exists(cache_path)){
        fs::create_directories(cache_path);
    }
    //init cacheSize with already existing files
    this->fillImgVect(cache_path);
    cacheSize = this->sizeOfCache();
    this->emptyImgVect();
}

void Shredder::updateSizeCache (uint_fast64_t fSize){
    if (write(sizePipe[writeEndPipe], &fSize, sizeof(uint_fast64_t)) == -1){
        perror("[Shredder::updateSizeCache] Write on sizePipe[writeEndPipe] take error");
    }
}

void Shredder::waitUntilFullCache (){
    timespec t{5, 0};
    sigset_t sigmask;
    sigfillset(&sigmask);
    int ready;

    while (true){
        ready = ppoll(&pollFd, 1, &t, &sigmask);
        switch (ready){
            case -1:
                perror("[Shredder::waitUntilFullCache]: ");
                exit(EX_OSERR);
            case 0:
                continue;
                break;
            default:
                //siccome guardiamo solo un file descriptor, c'è qualcosa da leggere
                return;
        }
    }
}

void Shredder::elaboratePipe (){
    uint_fast64_t size;
    bool end = false;
    do{
        if (read(sizePipe[readEndPipe], &size, sizeof(uint_fast64_t)) == -1){
            switch (errno){
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
        else{
            cacheSize += size;
            countFile++;
        }
    }while (!end);
}

void Shredder::freeSpace (){
    pthread_rwlock_wrlock(rwlock); //LOCK

    fillImgVect(cache_path); // obtain a vector with all the images
    reduceCacheUsage();

    pthread_rwlock_unlock(rwlock); //UNLOCK
}

string Shredder::sizeFormatted (uint_fast64_t size){
    if (size / MiB > 0)
        return fmt::format("{:.{}f} MiB", (float) (size) / MiB, 2);
    else if (size / KiB > 0)
        return fmt::format("{:.{}f} KiB", (float) (size) / KiB, 2);
    else
        return to_string(size) + " B";
}