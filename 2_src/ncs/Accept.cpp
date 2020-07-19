//
// Created by alfystar on 19/07/20.
//

#include "Accept.h"

using namespace NCS;
Accept::Accept (NCS::Queue *q){
	newData.lock();
	this->q = q;
	t1 = new std::thread(thListener, this);
	std::cout << "Hello World " << SERV_PORT << "\n";

}

void Accept::thListener (NCS::Accept *a){

	for(;;){
		// crea il socket di ascolto del processo verso la rete
		if ((a->listensd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
			perror ("errore in socket");
			exit (1);
		}

		//Rende riutilizzabile la porta
		int enable = 1;
		if (setsockopt (a->listensd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
			perror ("setsockopt(SO_REUSEADDR) failed");


		// Configuro i parametri con la quale deve funzionare la socket, attraverso la bind
		memset ((void *)& a->servaddr, 0, sizeof (servaddr));
		a->servaddr.sin_family = AF_INET;
		a->servaddr.sin_addr.s_addr = htonl (INADDR_ANY); /* il server accetta
        connessioni su una qualunque delle sue intefacce di rete */
		a->servaddr.sin_port = htons (SERV_PORT); /* numero di porta del server */

		// assegna l'indirizzo e la porta da far guardare alla socket
		if ((bind (a->listensd, (struct sockaddr *)& a->servaddr, sizeof (servaddr))) < 0){
			perror ("errore in bind");
			exit (1);
		}

		// Comunico al SO la dimensione della coda di attesa che deve gestire
		if (listen (a->listensd, BACKLOG) < 0){
			perror ("errore in listen");
			exit (1);
		}

		// Attende sia presente almeno una connessione (che ha già superato l'handShake a 3 vie)
		// Si ottiene un Soket connesso
		if ((a->connsd = accept (a->listensd, (struct sockaddr *)NULL, NULL)) < 0){
			perror ("errore in accept");
			exit (1);
		}

		a->lastFd = a->connsd;
		a->newData.unlock();
	}

}

int Accept::getLastFd (){
	newData.lock();
	return lastFd;
}
