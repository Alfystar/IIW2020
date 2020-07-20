//
// Created by alfystar on 19/07/20.
//

#include "Queue.h"

using namespace NCS;

Queue::Queue(){
	if(pipe2(waitPipe, O_DIRECT | O_NONBLOCK))
		perror("Queue::Queue error create waitPipe:");
	if(pipe2(readyPipe, O_DIRECT))
		perror("Queue::Queue error create readyPipe:");

	pollList = (struct pollfd *)calloc(MAX_CON, sizeof(struct pollfd));
	connectionList = (Connection **)calloc(MAX_CON, sizeof(Connection **));

	connectionList[0] = new Connection(waitPipe[readEnd]);
	connectionList[0]->compilePollFD(&pollList[0]);
	nextPoll = 1;

	tDisp = new std::thread(thDispatcher, this);

}

void Queue::thDispatcher(Queue *q){

	std::cout << "Queue::thDispatcher start work\n";
	timespec t {1, 0};

	int ready;
	sigset_t sigmask;
	sigfillset(&sigmask);
	while(true){
		ready = ppoll(q->pollList, q->nextPoll, &t, &sigmask);
		if(ready == 0){ //Time out
			std::cout << "Queue::thDispatcher ppoll time-out 1 s\n";
		}
		else if(ready == -1){
			perror("Queue::thDispatcher ppoll error:");
			exit(-1);
		}
		else{ //Qualcosa da fare:
			for(int i = 0; ready > 0; i++){
				int bitMask = q->pollList[i].revents;
				if(bitMask & POLLIN){ //Dato disponibile
					ready--;
					if(i == 0){ //Stiamo vedendo la ToWaitingPipe e c'è da leggere
						q->popWaitingCon();
					}
					else{
						q->pushReadyCon(i);
					}
				}
				else if(bitMask & POLLRDHUP){ // Testo se il keep-alive ha tagliato la connessione
					ready--;
					q->unValidCon(i);
				}
				else{
					std::bitset <16> x(bitMask);
					std::cout << "Queue::thDispatcher poll bitMask = " << x << '\n';
				}
			}
		}
	}

}

void Queue::pushWaitCon(Connection *con){
	pushPipe(waitPipe[writeEnd], con);
}

void Queue::popWaitingCon(){

	bool end = false;
	do{
		Connection *c = nullptr;
		if(read(waitPipe[readEnd], &c, sizeof(Connection *)) == -1){
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
			connectionList[nextPoll] = c;
			connectionList[nextPoll]->compilePollFD(&pollList[nextPoll]);
			nextPoll++;
		}
	}
	while(!end);

}

void Queue::pushReadyCon(int index){
	pushPipe(readyPipe[writeEnd], connectionList[index]);
	reduceList(index);
}

Connection *Queue::popReadyCon(){
	Connection *ret;
	if(read(readyPipe[readEnd], &ret, sizeof(Connection *)) == -1){
		switch(errno){
			default:
				perror("Queue::popWaitingCon pipe write error:");
				sleep(1);
				exit(-1);
		}
	}
	return ret;
}

void Queue::unValidCon(int index){
	Connection *c = reduceList(index);
	delete c;
}

// Funzione NON rientrante (ma tanto deve essere chiamata sempre da solo thDispatcher)
Connection *Queue::reduceList(int index){
	Connection *trash = connectionList[index];
	//Copio l'ultimo nello slot liberato
	connectionList[index] = connectionList[nextPoll - 1];
	memcpy(&pollList[index], &pollList[nextPoll - 1], sizeof(struct pollfd));
	//Riduco la dimensione della coda
	nextPoll--;
	return trash;
}

inline void Queue::pushPipe(int pipeWriteEnd, Connection *con){
// Dovendo scrivere 1 puntatore, ovvero 8 byte, l'operazione risulta atomica
	// e a meno di errori di altra natura è impossibile essere bloccati per un SEGNALE
	if(write(pipeWriteEnd, (void *)&con, sizeof(Connection *)) == -1){
		perror("Queue::pushReadyCon pipe write error:");
		sleep(1);
		exit(-1);
	}
}
