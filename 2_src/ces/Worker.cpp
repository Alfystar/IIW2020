//
// Created by alfystar on 20/07/20.
//

#include "Worker.h"

using namespace CES;

Worker::Worker(string &name){
	q = NCS::ncsGetQueue();
	myName = name;
	tJob = new std::thread(thWorker, this);

}

void Worker::thWorker(Worker *j){
	std::cout << "Worker::thWorker " << j->myName << " Start work\n";
	HttpMgt httpMgt;
	NCS::Connection *c;
	for(;;){
		c = j->q->popReadyCon();
		Action ret = httpMgt.connectionRequest(c);
		switch(ret){
			case ConClosed:
				// Connessione chiusa dentro httpMgt
				break;
			case RequestComplete:
			default:
				j->q->pushWaitCon(c);
				break;
		}
	}
}