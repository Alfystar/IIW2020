//
// Created by filippob on 13/09/20.
//

#ifndef HTTP_IMAGESERVER_SYNCUTILITIES_H
#define HTTP_IMAGESERVER_SYNCUTILITIES_H

#include <mutex>          // std::mutex

#include <thread>
#include <malloc.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fcntl.h>
#include <unistd.h>

#define readEndPipe 0
#define writeEndPipe 1

extern pthread_rwlock_t *rwlock; //lock for shredder and workers
extern pthread_rwlockattr_t *attr;

extern std::mutex openMutex;    // Mutex per rendere atomica l'apertura dei file

int initShredderLock ();

#endif //HTTP_IMAGESERVER_SYNCUTILITIES_H
