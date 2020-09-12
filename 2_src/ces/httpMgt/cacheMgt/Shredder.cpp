//
// Created by filippob on 12/09/20.
//

#include <vector>
#include "Shredder.h"

using namespace CES;

/// ImgData, used for sorting and working on filesystem

ImgData::ImgData(string p) { //otteniamo un oggetto con i dati per il sort
    path = p;

    if (stat(p.c_str(), buf)){
        perror("File Stats error: ");
        path = nullptr;
        buf = nullptr;
    }

}

/// Shredder functions


Shredder * Shredder::instance = nullptr;

Shredder &Shredder::getInstance() {
    cout << "Got Instance";
    if(!instance)
        instance = new Shredder();
    return *instance;
}

Shredder::Shredder() {

    tShr = new thread(threadShr, this);
}

[[noreturn]] void Shredder::threadShr(Shredder *s) {
    cout << "Shredder started\n";
    string cache_path = "web/cache";

    for(;;){

        // lock shredder_mutex

        cout << "Verifying size of cache";

        s->fillImgsVect(cache_path);





        this_thread::sleep_for(chrono::seconds(1));
    }
}


void Shredder::fillImgsVect(string &p){
    DIR* cacheDir = opendir(p.c_str());
    struct dirent *dir;
    if (cacheDir) {
        while ((dir = readdir(cacheDir)) != nullptr) {
           // ImgData i = new ImgData(std::string(dir->d_name));
           // imgsVect.push_back(i);
        }
    }
}