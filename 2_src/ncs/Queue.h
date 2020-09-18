//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_QUEUE_H
#define HTTP_IMAGESERVER_QUEUE_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif


#include <iostream>
#include <cstring>
#include <bitset>

//poll directive
#include <signal.h>
#include <poll.h>

//pipe directive
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

#define readEnd 0
#define writeEnd 1


//thread directive
#include <thread>         // std::thread
#include <atomic>

#include <Log.h>

#include "ncsDefine.h"
#include <Connection.h>
#include "timeSpecOp.h"

namespace NCS {
    class Queue {
    private:
        std::thread *tDisp;
        int waitPipe[2], readyPipe[2];

        // pollList contiene dati per la poll
        // connectionList è il nostro oggetto Connesione per mantenere traccia
        int nextPoll; //Primo indice libero / n° elementi presenti
        struct pollfd *pollList;    //Array necessario alla poll
        Connection **connectionList; // Array di PUNTATORI

    public:
        Queue();

        Connection *popReadyCon();

        void pushWaitCon(Connection *con);

    private:
        static void thDispatcher(Queue *q);

        void popWaitingCon();

        void pushReadyCon(int index);

        void unValidCon(int index);

        Connection *reduceList(int index);

        void pushPipe(int pipeWriteEnd, Connection *con);
    };
}

#endif //HTTP_IMAGESERVER_QUEUE_H
