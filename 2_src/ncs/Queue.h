//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_QUEUE_H
#define HTTP_IMAGESERVER_QUEUE_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif

//poll include
#include <signal.h>
#include <poll.h>

//pipe include
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

#define readEnd 0
#define writeEnd 1


#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <atomic>

#include "ncsDefine.h"
#include "Connection.h"

namespace NCS{
	class Queue{
	private:
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

		void unValidCon(int index)
	};
}

#endif //HTTP_IMAGESERVER_QUEUE_H
