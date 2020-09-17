//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_ACCEPT_H
#define HTTP_IMAGESERVER_ACCEPT_H


#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <mutex>

#include <sys/time.h>
#include <sys/resource.h>


#include "Queue.h"
#include <Connection.h>
#include "ncsDefine.h"
#include <Log.h>
#include "../GLOBAL_DEF.h"

//#define BACKLOG 10
//#define SERV_PORT   8080


namespace NCS{
	class Accept{
	private:
		Queue *q;
		std::thread *tListener;

		int listensd;
		struct sockaddr_in servaddr;

	public:
		Accept(Queue *q);

	private:
		static void thListener(Accept *a);

		void sockInit();
	};
}

#endif //HTTP_IMAGESERVER_ACCEPT_H
