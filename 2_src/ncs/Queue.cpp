//
// Created by alfystar on 19/07/20.
//

#include "Queue.h"

using namespace NCS;

Queue::Queue(){
	if(pipe2(waitPipe, O_DIRECT | O_NONBLOCK))
		perror("Queue::Queue error create waitPipe:");
	if(pipe2(readyPipe, O_DIRECT | O_NONBLOCK))
		perror("Queue::Queue error create readyPipe:");

	pollList = (struct pollfd *)calloc(MAX_CON, sizeof(struct pollfd));
	connectionList = (Connection **)calloc(MAX_CON, sizeof(Connection **));

	pollList[0].fd = waitPipe[readEnd];
	pollList[0].events = POLLIN;
	nextPoll = 1;

}

Connection *Queue::popReadyCon(){

	return nullptr;
}

void Queue::pushWaitCon(Connection *con){
	// Dovendo scrivere 1 puntatore, ovvero 8 byte, l'operazione risulta atomica
	// e a meno di errori di altra natura è impossibile essere bloccati per un SEGNALE
	if(write(waitPipe[writeEnd], (void *)con, sizeof(Connection *)) == -1){
		perror("Queue::pushWaitCon pipe write error:");
		sleep(1);
		exit(-1);
	}
}

void Queue::thDispatcher(Queue *q){

}

void Queue::popWaitingCon(){
	Connection *conBuff;
	bool end = false;
	do{
		if(read(waitPipe[readEnd], &connectionList[nextPoll], sizeof(Connection *)) == -1){
			switch(errno){
				case EAGAIN:
					end = true;
					break;
				default:
					perror("Queue::popWaitingCon pipe write error:");
					sleep(1);
					exit(-1);
			}
		}
		else{
			connectionList[nextPoll]->compilePollFD(&pollList[nextPoll]);
			nextPoll++;
		}
	}
	while(!end);

}

void Queue::pushReadyCon(int index){
	//todo: quando la connessione "i" è pronta per essere elaborata, la tolgo dalla coda
	// comprimo la memoria sia di connectionList che di pollList (spostando gli ultimi all'indice liberato)
	// e aggiungo alla pipe delle ready

	nextPoll--;
}

void Queue::unValidCon(int index){
	//todo: quando la connessione "i" è invalida (tipo keep-alive scaduto) la elimino e
	// comprimo la memoria sia di connectionList che di pollList (spostando gli ultimi all'indice liberato)

	nextPoll--;
}
