//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_QUEUE_H
#define HTTP_IMAGESERVER_QUEUE_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif

#include <signal.h>
#include <poll.h>

#include <iostream>       // std::cout
#include <thread>         // std::thread
#include "Connection.h"

namespace NCS{
	class Queue{
	private:
		int waitPipe[2], readyPipe[2];

		// pollList contiene dati per la poll
		// connectionList è il nostro oggetto Connesione per mantenere traccia
		struct pollfd *pollList;
		Connection *connectionList;

	public:
		Queue();
		Connection *popReadyCon();
		void pushWaitCon(Connection * con);
	private:
		static void thDispatcher(Queue *q);
		Connection *popWaitingCon();
		void pushReadyCon(Connection * con);
	};
}

#endif //HTTP_IMAGESERVER_QUEUE_H
