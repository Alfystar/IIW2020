//
// Created by alfystar on 20/07/20.
//

#include "CES.h"

using namespace CES;

int CES::nWorkers = 0;
Shredder *shr = nullptr;
Worker **CES::workers = nullptr; //worker per la gestione delle richieste

void CES::initCES (int n){
    #ifdef IMAGE_RESIZE
    shr = Shredder::getInstance();
    sleep(1);
    #endif
    CES::nWorkers = n;
    workers = (Worker **) calloc(n, sizeof(Worker **));
    string name;

    for (int i = 0; i < n; ++i){
        name = fmt::format("Worker {}", i);
        workers[i] = new Worker(name);
    }
    cout << "[CES::initCES] Created " << n << " Workers\n";
}
