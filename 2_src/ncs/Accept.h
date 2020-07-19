//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_ACCEPT_H
#define HTTP_IMAGESERVER_ACCEPT_H

#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <mutex>

#include "Queue.h"
#include "Connection.h"

#define BACKLOG 10
#define SERV_PORT   8080
#define MAXLINE 1024


namespace NCS{
	class Accept{
	private:
		Queue *q;
		std::thread *t1;

		std::mutex newData;

		int lastFd;

		int listensd, connsd;
		struct sockaddr_in servaddr;
		char buff[MAXLINE];
	public:
		Accept(Queue *q);
		int getLastFd();

	private:
		static void thListener(Accept *a);

	};
}

#endif //HTTP_IMAGESERVER_ACCEPT_H
