//
// Created by alfystar on 17/09/20.
//

#ifndef HTTP_IMAGESERVER_GLOBAL_DEF_H
#define HTTP_IMAGESERVER_GLOBAL_DEF_H


#include <sysexits.h>

// Accept settings
#define BACKLOG 10

// Shreader setting
#define FILE_SIZE_LIMIT 10*1024*1024 // 10 MB of images allowed before halving

// Cache system
#define CACHE_PATH "./web/cache"


extern volatile int SERV_PORT; //8080 di default
//#define SERV_PORT   8080

#endif //HTTP_IMAGESERVER_GLOBAL_DEF_H
