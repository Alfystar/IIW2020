//
// Created by alfystar on 19/07/20.
//

#include "NCS.h"

using namespace NCS;

Queue *NCS::queueObj = nullptr;

Accept *NCS::acceptObj = nullptr;

std::mutex NCS::queueLock;
std::mutex NCS::acceptLock;

Queue *NCS::ncsGetQueue (){
    const std::lock_guard <std::mutex> lock(queueLock);
    if (queueObj) //Oggetto già definito
        return queueObj;

    queueObj = new Queue();
    acceptObj = new Accept(queueObj);
    return queueObj;
}

Accept *NCS::ncsGetAccept (){
    const std::lock_guard <std::mutex> lock(acceptLock);
    if (acceptObj) //Oggetto già definito
        return acceptObj;

    acceptObj = new Accept(queueObj);
    return acceptObj;
}
