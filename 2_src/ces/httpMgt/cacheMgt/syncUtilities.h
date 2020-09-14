//
// Created by filippob on 13/09/20.
//

#ifndef HTTP_IMAGESERVER_SYNCUTILITIES_H
#define HTTP_IMAGESERVER_SYNCUTILITIES_H

#include <mutex>          // std::mutex

#include <pthread.h>
#include <malloc.h>

extern pthread_rwlock_t *rwlock; //lock for shredder and workers
extern pthread_rwlockattr_t *attr;

extern std::mutex openMutex;

int initShredderLock();

int initResourceLock();

int initOpenMutex();


#endif //HTTP_IMAGESERVER_SYNCUTILITIES_H
