//
// Created by alfystar on 19/07/20.
//

#include "Connection.h"

using namespace NCS;

std::atomic<unsigned long> Connection::count(0);

Connection::Connection(int fd) : Connection(fd, NULL, 0) {
    cType = internalConnect;
}


Connection::Connection(int fd, struct sockaddr *sockInfo, socklen_t socklen) {
    this->fd = fd;
    this->socklen = socklen;
    if (sockInfo) {
        memcpy(&this->sockInfo, sockInfo, sizeof(struct sockaddr));
        if (sockInfo->sa_family == AF_INET || sockInfo->sa_family == AF_INET6)
            cType = tcpConnect;
    } else
        memset(&this->sockInfo, 0, sizeof(struct sockaddr));

    count++;
}

Connection::~Connection() {
    count--;
    close(fd);
    Log::out << "Connection fd: " << fd << "was closed" << endl;
}

void Connection::compilePollFD(struct pollfd *pollFd) {
    pollFd->fd = fd;
    pollFd->events = POLLIN | POLLRDHUP;
}

unsigned long Connection::activeConnection() {
    return count;
}

Connection::ConnectType Connection::getType() {
    return tcpConnect;
}

int Connection::sendData(const void *data, int datalen) {
    const char *ptr = static_cast<const char *>(data);
    while (datalen > 0) {
        int bytes = send(this->fd, ptr, datalen, MSG_NOSIGNAL);
        if (bytes <= 0) return -1;
        ptr += bytes;
        datalen -= bytes;
    }
    return 0;
}

int Connection::sendStr(const std::string &s) {
    return sendData(s.c_str(), s.size());
}

NCS::Connection::httpHeader *Connection::readHttpHeader() {
    if (cType == unknown || cType == internalConnect)
        return nullptr;

    char buff[MAXLINE];

    int bRead, index = 0, lIndex;
    char *endHeader;
    while (true) {
        lIndex = index;
        bRead = read(fd, &buff[index], MAXLINE - index);
        if (bRead < 0) {
            perror("Connection::readHttpHeader read reach error:");
            return nullptr;
        }
        index += bRead;

        endHeader = strstr(&buff[max(0, lIndex - 4)], "\r\n\r\n");
        if (endHeader) {
            endHeader += 4;
            break;
        } else if (bRead == 0) { //Raggiunto end-of-file o fine del buffer
            return nullptr;
        }
    }
    //Definiamo una stringa dal buffer letto
    string str(buff, endHeader - buff);

    // Trasformiamo la stringa in uno streambuf
    //Creiamo dallo streambuf un inputStream per farlo analizzare
    stringbuf sBuf(str);
    istream istream1(&sBuf);

    // Definiamo i campi di risposta
    auto *ret = new httpHeader();
    //Analizziamo la richiesta e la salviamo sui campo
    if (SimpleWeb::RequestMessage::parse(istream1, ret->method, ret->path, ret->query_string, ret->version, ret->cim)) {
        cType = httpConnect;
    } else {
        delete ret;
        return nullptr; //Il messaggio ricevuto non era http
    }
    return ret;
}

