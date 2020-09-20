//
// Created by alfystar on 19/07/20.
//

#include "Queue.h"

using namespace NCS;

Queue::Queue (){
    if (pipe2(waitPipe, O_DIRECT | O_NONBLOCK))
        perror("[Queue::Queue] Error create waitPipe:");
    if (pipe2(readyPipe, O_DIRECT))
        perror("[Queue::Queue] Error create readyPipe:");

    pollList = (struct pollfd *) calloc(MAX_CON, sizeof(struct pollfd));
    connectionList = (Connection **) calloc(MAX_CON, sizeof(Connection **));

    connectionList[0] = new Connection(waitPipe[readEnd]);
    connectionList[0]->compilePollFD(&pollList[0]);
    nextPoll = 1;

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
            // All ok
        }
        else if (ready == -1){
            perror("[Queue::thDispatcher] ppoll error:");
            exit(-1);
        }
        else{ //Qualcosa da fare:
            for (int i = 0; ready > 0; i++){
                int bitMask = q->pollList[i].revents;
                if (bitMask & POLLIN){ //Dato disponibile
                    ready--;
                    if (i == 0){
                        //Siamo stati svegliati dalla la ToWaitingPipe e c'è da leggere dati
                        q->popWaitingCon(); //leggo i dati nella pipe
                    }
                    else{
                        //Una COM ha un dato Disponibile
                        q->pushReadyCon(i);
                    }
                }
                else if (bitMask & POLLRDHUP){
                    // La CON è stata tagliata dal keep-alive
                    ready--;
                    q->unValidCon(i);
                }
                else{
                    // Un evento non specificato è avvenuto al FD
                    #ifdef DEBUG_LOG
                    if (bitMask){    // Un segnale ha causato uscita dalla poll
                        std::bitset <16> x(bitMask);
                        Log::db << "[Queue::thDispatcher] poll bitMask = " << x << '\n';
                    }
                    else{
                        // Questo FD non è stato toccato
                    }
                    #endif
                }
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
        Connection *c = nullptr;
        if (read(waitPipe[readEnd], &c, sizeof(Connection *)) == -1){
            switch (errno){
                case EAGAIN:
                    // la lettura vorrebbe bloccare, ma noi l'abbiamo segnata non bloccante
                    // Quindi la pipe è stata svuotata
                    end = true; //termino il while perchè ho lettot tutto
                    break;
                default:
                    perror("[Queue::popWaitingCon] Pipe write error:");
                    sleep(1);
                    exit(-1);
            }
        }
        else{
            connectionList[nextPoll] = c;
            connectionList[nextPoll]->compilePollFD(&pollList[nextPoll]);
            nextPoll++;
        }
    }while (!end);
}

void Queue::pushReadyCon (int index){
    pushPipe(readyPipe[writeEnd], connectionList[index]);
    reduceList(index);
}

Connection *Queue::popReadyCon (){
    Connection *ret;
    if (read(readyPipe[readEnd], &ret, sizeof(Connection *)) == -1){
        switch (errno){
            default:
                perror("[Queue::popWaitingCon] Pipe write error:");
                sleep(1);
                exit(-1);
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
    //Copio l'ultimo nello slot liberato
    connectionList[index] = connectionList[nextPoll - 1];
    memcpy(&pollList[index], &pollList[nextPoll - 1], sizeof(struct pollfd));
    //Riduco la dimensione della coda
    nextPoll--;
    return trash;
}

inline void Queue::pushPipe (int pipeWriteEnd, Connection *con){
    // Dovendo scrivere 1 puntatore, ovvero 8 byte, l'operazione risulta atomica
    // e a meno di errori di altra natura è impossibile essere bloccati per un SEGNALE
    //TODO: Gestire EAGAIN dovuto al fatto che una delle pipe è NON_BLOCCANTE
    if (write(pipeWriteEnd, (void *) &con, sizeof(Connection *)) == -1){
        perror("[Queue::pushReadyCon] Pipe write error:");
        sleep(1);
        exit(-1);
    }
}
