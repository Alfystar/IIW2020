//
// Created by alfystar on 19/07/20.
//

#include "Connection.h"

using namespace NCS;

std::atomic <unsigned long> Connection::count(0);

Connection::Connection(int fd) : Connection(fd, NULL, 0){

}


Connection::Connection(int fd, struct sockaddr *sockInfo, socklen_t socklen){
	this->fd = fd;
	this->socklen = socklen;
	if(sockInfo)
		memcpy(&this->sockInfo, sockInfo, sizeof(struct sockaddr));
	else
		memset(&this->sockInfo, 0, sizeof(struct sockaddr));
	count++;
}

Connection::~Connection(){
	count--;
}

void Connection::compilePollFD(struct pollfd *pollFd){
	pollFd->fd = fd;
	pollFd->events = POLLIN | POLLRDHUP;
}

unsigned long Connection::activeConnection(){
	return count;
}

Connection::connectType Connection::getType(){
	if(sockInfo.sa_family != 0)
		return Connection::tcpConnect;
	if(fd < 0)
		return Connection::unknow;
	return Connection::internallConnect;
}
