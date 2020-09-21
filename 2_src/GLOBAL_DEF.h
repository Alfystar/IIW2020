//
// Created by alfystar on 17/09/20.
//

#ifndef HTTP_IMAGESERVER_GLOBAL_DEF_H
#define HTTP_IMAGESERVER_GLOBAL_DEF_H


#include <sysexits.h>

// main startUp
#define nWorker get_nprocs()  //Per avere il doppio dei core e gestire le situazioni di wait forzate


// Accept settings
#define BACKLOG 100000

// Queue setting
#define MAX_CON 4096

// Shreader setting
#define KiB 1024
#define MiB (1024*KiB)
#define FILE_SIZE_LIMIT (10*MiB) // 10 MiB of images allowed before halving

// Cache system
#define CACHE_PATH "./web/sys/cache"


extern volatile int SERV_PORT; //8080 di default
//#define SERV_PORT   8080

#endif //HTTP_IMAGESERVER_GLOBAL_DEF_H