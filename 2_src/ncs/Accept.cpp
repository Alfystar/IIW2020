//
// Created by alfystar on 19/07/20.
//

#include "Accept.h"

using namespace NCS;

Accept::Accept(NCS::Queue *q){
	newData.unlock();
	newData.lock();
	this->q = q;

	sockInit();

	t1 = new std::thread(thListener, this);
	std::cout << "Hello Accept " << SERV_PORT << "\n";

}

void Accept::thListener(NCS::Accept *a){

	for(;;){

		// Attende sia presente almeno una connessione (che ha già superato l'handShake a 3 vie)
		// Si ottiene un Soket connesso
		if((a->connsd = accept(a->listensd, (struct sockaddr *)NULL, NULL)) < 0){
			switch(errno){
				case EMFILE:
					//todo: add debug print
					sleep(1);   //Sicuramente ci sono tante connessioni da gestire ancora
					break;
				default:
					perror("Accept::thListener errore in accept");
					exit(1);
			}
		}

		a->lastFd = a->connsd;
		a->newData.unlock();
	}

}

int Accept::getLastFd(){
	newData.lock();
	return lastFd;
}

void Accept::sockInit(){

	struct rlimit limit;
	if(getrlimit(RLIMIT_NOFILE, &limit))
		perror("Accept::sockInit errore in getrlimit:");

	limit.rlim_cur = std::min((rlim_t)MAX_CON, limit.rlim_max);
	if(setrlimit(RLIMIT_NOFILE, &limit))
		perror("Accept::sockInit errore in setrlimit:");



	// crea il socket di ascolto del processo verso la rete
	if((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Accept::sockInit errore in socket");
		exit(1);
	}

	//Rende riutilizzabile la porta
	int enable = 1;
	if(setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		perror("Accept::sockInit setsockopt(SO_REUSEADDR) failed");

	// Configuro i parametri con la quale deve funzionare la socket, attraverso la bind
	memset((void *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta
        connessioni su una qualunque delle sue intefacce di rete */
	servaddr.sin_port = htons(SERV_PORT); /* numero di porta del server */

	// assegna l'indirizzo e la porta da far guardare alla socket
	if((bind(listensd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0){
		perror("Accept::sockInit errore in bind");
		exit(1);
	}


	// Comunico al SO la dimensione della coda di attesa che deve gestire
	if(listen(listensd, BACKLOG) < 0){
		perror("Accept::sockInit errore in listen");
		exit(1);
	}
}
