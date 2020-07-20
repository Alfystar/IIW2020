//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_CONNECTION_H
#define HTTP_IMAGESERVER_CONNECTION_H


#include <cstdio>
#include <cstring>
#include <atomic>

#include <poll.h>
#include <sys/socket.h>

namespace NCS{
	class Connection{
	public:

		enum connectType{internallConnect, tcpConnect, unknow};

		int fd;
		struct sockaddr sockInfo;
		socklen_t socklen;
	private:
		static std::atomic <unsigned long> count;
	public:
		Connection(int fd);

		Connection(int fd, struct sockaddr *sockInfo, socklen_t socklen);

		~Connection();

		unsigned long activeConnection();

		void compilePollFD(struct pollfd *pollFd);

		connectType getType();

	};
}

#endif //HTTP_IMAGESERVER_CONNECTION_H
