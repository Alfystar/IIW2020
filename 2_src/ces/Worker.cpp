//
// Created by alfystar on 20/07/20.
//

#include "Worker.h"

using namespace CES;

Worker::Worker (string &name){
    q = NCS::ncsGetQueue();
    myName = name;
    tJob = new std::thread(thWorker, this);
}

[[noreturn]] void Worker::thWorker (Worker *j){
    pthread_setname_np(pthread_self(), j->myName.c_str());
    sleep(1);
    #ifdef DEBUG_LOG
    Log::db << "[Worker::thWorker] " << j->myName << " Start work\n";
    #endif
    HttpMgt httpMgt;
    NCS::Connection *c;
    Action ret;
    for (;;){
        c = j->q->popReadyCon();

        //todo cancellare
        char hex_string[20];
        sprintf(hex_string, "%p", (void *) c); //convert number to hex
        cout << (j->myName + " get connect connection: " + string(hex_string) + "\n");
        ret = httpMgt.connectionRequest(c);

        switch (ret){
            case ConClosed:
                //                //todo cancellare
                cout << (j->myName + " delete connect: " + string(hex_string) + "\n");
                delete c;
                c = nullptr;
                // Connessione chiusa dentro httpMgt
                break;
            default:
                //                //todo cancellare
                cout << (j->myName + " push connect: " + string(hex_string) + "\n");
                j->q->pushWaitCon(c);
                c = nullptr;
                break;
        }
    }
}