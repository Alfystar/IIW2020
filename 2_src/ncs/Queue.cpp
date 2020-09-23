//
// Created by alfystar on 19/07/20.
//

#include "Queue.h"

using namespace NCS;

Queue::Queue (){
    //O_DIRECT
    if (pipe2(waitPipe, 0)){
        perror("[Queue::Queue] Error create waitPipe:");
        exit(EX_IOERR);
    }
    // Rendo NON BLOCCANTE solo l'estremo in lettura
    if (fcntl(waitPipe[readEnd], F_SETFL, O_NONBLOCK) < 0){
        perror("[Queue::Queue] Error make waitPipe[readEnd] O_NONBLOCK:");
        exit(EX_IOERR);
    }

    if ((fcntl(waitPipe[readEnd], F_SETPIPE_SZ, 1 * MiB)) == -1){
        perror("[Queue::Queue] Error to EncreIncreasease  waitPipe to 1Mib:");
        exit(EX_IOERR);
    }

    if (pipe2(readyPipe, 0)){
        perror("[Queue::Queue] Error create readyPipe:");
        exit(EX_IOERR);
    }

    if ((fcntl(readyPipe[readEnd], F_SETPIPE_SZ, 1 * MiB)) == -1){
        perror("[Queue::Queue] Error to Increase  readyPipe to 1Mib:");
        exit(EX_IOERR);
    }

    connectionList[0] = new Connection(waitPipe[readEnd]);
    connectionList[0]->compilePollFD(&pollList[0]);

    nextPoll = 1;   //Nella posizione 0 c'è il FD della waitPipe[readEnd]

    tDisp = new std::thread(thDispatcher, this);
}

void Queue::thDispatcher (Queue *q){
    pthread_setname_np(pthread_self(), "Queue");

    std::cout << "[Queue::thDispatcher] Start work\n";
    timespec t{1, 0};
    int ready;
    sigset_t sigmask;
    sigfillset(&sigmask);

    while (true){
        ready = ppoll(q->pollList, q->nextPoll, &t, &sigmask);
        if (ready == 0){ //Time out
            #ifdef DEBUG_LOG
            //Log::db << "[Queue::thDispatcher] poll Time out\n";
            #endif
            continue;
        }
        if (ready == -1){
            perror("[Queue::thDispatcher] ppoll error:");
            exit(EX_IOERR);
        }
        //Qualcosa da fare:
        for (int i = q->nextPoll - 1; ready > 0 && i >= 0; i--){
            int bitMask = q->pollList[i].revents;
            if (bitMask){
                ready--;
                if (bitMask == POLLIN){ //Dato disponibile
                    if (i == 0){
                        //Siamo stati svegliati dalla la ToWaitingPipe e c'è da leggere dati
                        q->popWaitingCon(); //leggo i dati nella pipe
                        continue;
                    }
                    else{
                        //Una COM ha un dato Disponibile
                        q->pushReadyCon(i);
                        continue;
                    }
                }

                if (bitMask & POLLHUP){
                    #ifdef DEBUG_VERBOSE
                    cout << "[Queue::thDispatcher] POLLHUP bit: Fin & Ack has been recived and sendGet\n";
                    #endif
                    q->unValidCon(i);
                    continue;
                }
                if (bitMask & POLLERR){
                    #ifdef DEBUG_VERBOSE
                    cout
                            << "[Queue::thDispatcher] POLLERR bit: Error condition (only returned in revents; ignored in events)\n";
                    #endif
                    q->unValidCon(i);
                    continue;
                }
                #ifdef DEBUG_LOG
                std::bitset <16> x(bitMask);
                Log::db << "[Queue::thDispatcher] poll bitMask = " << x << '\n';
                #endif
            }
        }
    }
}

void Queue::pushWaitCon (Connection *con){
    pushPipe(waitPipe[writeEnd], con);
}

void Queue::popWaitingCon (){
    bool end = false;
    do{
        Connection *c;
        if (read(waitPipe[readEnd], &c, sizeof(Connection *)) == -1){
            switch (errno){
                case EAGAIN:
                    // La lettura vorrebbe bloccare, ma noi l'abbiamo segnata non bloccante
                    //    => segue che la pipe è stata svuotata e non è necessario leggere altro
                    end = true;
                    break;
                default:
                    perror("[Queue::popWaitingCon] Pipe write error:");
                    sleep(1);
                    exit(-1);
            }
        }
        else{
            if (nextPoll == MAX_CON - 1){
                cerr << "[Queue::popWaitingCon] try to add new wait connection but queue end!\nConnection will close";
                delete c;
            }
            else{
                connectionList[nextPoll] = c;
                connectionList[nextPoll]->compilePollFD(&pollList[nextPoll]);
                nextPoll++;
            }
        }
    }while (!end);
}

void Queue::pushReadyCon (int index){
    Connection *c = reduceList(index);
    pushPipe(readyPipe[writeEnd], c);
}

Connection *Queue::popReadyCon (){
    Connection *ret;
    if (read(readyPipe[readEnd], &ret, sizeof(Connection *)) == -1){
        switch (errno){
            default:
                perror("[Queue::popWaitingCon] Pipe write error:");
                sleep(1);
                exit(EX_IOERR);
        }
    }
    return ret;
}

void Queue::unValidCon (int index){
    Connection *c = reduceList(index);
    delete c;
}

// Funzione NON rientrante (ma tanto deve essere chiamata sempre da solo thDispatcher)
Connection *Queue::reduceList (int index){
    Connection *trash = connectionList[index];

    // Copio l'ultimo nello slot liberato
    connectionList[index] = connectionList[nextPoll - 1];
    pollList[index] = pollList[nextPoll - 1];

    // Cancello l'ultimo per evitare copie
    connectionList[nextPoll - 1] = 0;
    pollList[nextPoll - 1] = {};
    //    memcpy(&pollList[index], &pollList[nextPoll], sizeof(struct pollfd));
    //Riduco la dimensione della coda
    nextPoll--;

    return trash;
}

inline void Queue::pushPipe (int pipeWriteEnd, Connection *con){
    // Dovendo scrivere 1 puntatore, ovvero 8 byte, l'operazione risulta atomica
    // e a meno di errori di altra natura è impossibile essere bloccati per un SEGNALE
    if (write(pipeWriteEnd, (void *) &con, sizeof(Connection *)) == -1){
        perror("[Queue::pushPipe] Pipe write error:");
        sleep(1);
        exit(EX_IOERR);
    }
}
