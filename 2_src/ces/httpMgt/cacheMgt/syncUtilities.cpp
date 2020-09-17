//
// Created by filippob on 13/09/20.
//

#include "syncUtilities.h"

pthread_rwlock_t *rwlock = (pthread_rwlock_t *) (malloc(sizeof(pthread_rwlock_t)));
pthread_rwlockattr_t *attr = (pthread_rwlockattr_t *) (malloc(sizeof(pthread_rwlockattr_t)));
std::mutex openMutex;

int sizePipe[2];

int initSizePipe() {
    if (pipe2(sizePipe, O_DIRECT)) {
        perror("[initSizePipe]: ");
        return -1;
    }
    return 0;
}

int initShredderLock() {
    *rwlock = PTHREAD_RWLOCK_INITIALIZER;
    if (pthread_rwlockattr_setkind_np(attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP)) {
        perror("Error in pthread_rwlockattr_setkind_np: ");
        return -1;
    }
    if (pthread_rwlock_init(rwlock, attr)) {
        perror("Error in pthread_rwlock_init: ");
        return -1;
    }
    return 0;
}


