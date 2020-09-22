//
// Created by alfystar on 19/07/20.
//

#include "Connection.h"

using namespace NCS;

#ifdef COUNT_INSTANCE
std::atomic <unsigned long> Connection::count(0);
#endif

Connection::Connection (int fd) : Connection(fd, NULL, 0){
    cType = internalConnect;
}

Connection::Connection (int fd, struct sockaddr *sockInfo, socklen_t socklen){
    this->fd = fd;

    this->socklen = socklen;
    if (sockInfo){
        memcpy(&this->sockInfo, sockInfo, sizeof(struct sockaddr));
        if (sockInfo->sa_family == AF_INET || sockInfo->sa_family == AF_INET6)
            cType = tcpConnect;
    }
    else
        memset(&this->sockInfo, 0, sizeof(struct sockaddr));
    //todo cancelare
    if (cType == tcpConnect){
        char hex_string[20];
        sprintf(hex_string, "%p", (void *) this); //convert number to hex
        cout << ("Connection fd: " + to_string(fd) + " are created, memory: " + string(hex_string) + "\n");
    }
    #ifdef COUNT_INSTANCE
    count++;
    #endif
}

Connection::~Connection (){
    #ifdef COUNT_INSTANCE
    count--;
    #endif

    //todo cancelare
    char hex_string[20];
    sprintf(hex_string, "%p", (void *) this); //convert number to hex
    cout << ("Connection fd: " + to_string(fd) + " will closed, memory: " + string(hex_string) + "\n");
//    if(cType == tcpConnect || cType == httpConnect){
//        if(shutdown(fd,SHUT_RDWR))
//            perror("[Connection::~Connection] Shutdown get error");
//    }

    close(fd);
}

void Connection::compilePollFD (struct pollfd *pollFd){
    pollFd->fd = fd;
    pollFd->events = POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL;
}

#ifdef COUNT_INSTANCE

unsigned long Connection::activeConnection (){
    return count;
}

#endif

Connection::ConnectType Connection::getType (){
    return cType;
}

int Connection::sendData (const void *data, int datalen){
    const char *ptr = static_cast<const char *>(data);
    while (datalen > 0){
        int bytes = send(this->fd, ptr, datalen, MSG_NOSIGNAL);
        if (bytes == -1) {
            switch (errno){
                case EINTR:
                    bytes = 0;
                    break;
                case EPIPE:
                default:
                    return -1;
            }
        }
        ptr += bytes;
        datalen -= bytes;
    }
    return 0;
}

int Connection::sendStr (const std::string &s){
    return sendData(s.c_str(), s.size());
}

NCS::Connection::httpHeader *Connection::readHttpHeader (){
    if (cType == unknown || cType == internalConnect)
        return nullptr;

    char buff[MAXLINE];
    int bRead, index = 0, lIndex;
    char *endHeader;
    while (true){
        lIndex = index;
        bRead = read(fd, &buff[index], MAXLINE - index);
        if (bRead < 0){
            perror("[Connection::readHttpHeader] read reach error:");
            return nullptr;
        }
        index += bRead;

        endHeader = strstr(&buff[max(0, lIndex - 4)], "\r\n\r\n");
        if (endHeader){
            endHeader += 4;
            break;
        }
        else if (bRead == 0){ // Connessione troncata dall'altra metà
            return nullptr;
        }
    }
    //Definiamo una stringa dal buffer letto
    // Sottraendo i 2 puntatori otteniamo la lunghezza dei byte da copiare
    string str(buff, endHeader - buff);

    // Trasformiamo la stringa in uno streambuf
    //Creiamo dallo streambuf un inputStream per farlo analizzare
    stringbuf sBuf(str);
    istream istream1(&sBuf);

    // Definiamo i campi di risposta
    auto *ret = new httpHeader();
    //Analizziamo la richiesta e la salviamo sui campo
    if (SimpleWeb::RequestMessage::parse(istream1, ret->method, ret->path, ret->query_string, ret->version, ret->cim)){
        cType = httpConnect;
    }
    else{
        cout << "[Connection::readHttpHeader] delete message\n";
        //        delete ret;
        return nullptr; //Il messaggio ricevuto non era http
    }
    return ret;
}

