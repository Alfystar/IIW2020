//netinet
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_NCS_H
#define HTTP_IMAGESERVER_NCS_H

#include <mutex>
#include "Accept.h"
#include <Connection.h>
#include "Queue.h"

namespace NCS {
    extern Queue *queueObj;
    extern Accept *acceptObj;
    extern std::mutex queueLock;
    extern std::mutex acceptLock;
    Queue *ncsGetQueue ();   //"Singleton di Sistema"

    Accept *ncsGetAccept ();   //"Singleton di Sistema"

}

#endif //HTTP_IMAGESERVER_NCS_H
