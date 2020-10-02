//
// Created by alfystar on 19/07/20.
//

#include "Accept.h"

using namespace NCS;

Accept::Accept (NCS::Queue *q){
    this->q = q;
    sockInit();
    tListener = new std::thread(thListener, this);
}

void Accept::thListener (NCS::Accept *a){
    pthread_setname_np(pthread_self(), "Accepter");

    std::cout << "[Accept::thListener] Start work on PORT: " << SERV_PORT << "\n";
    Connection *c = nullptr;
    int connsd;
    struct sockaddr info;
    socklen_t lenSockAddr = sizeof(info);
    int count = 0;
    for (;;){
        // Attende sia presente almeno una connessione (che ha già superato l'handShake a 3 vie)
        // Si ottiene un Soket connesso
        memset(&info, 0, sizeof(struct sockaddr));
        if ((connsd = accept(a->listensd, &info, &lenSockAddr)) == -1){
            switch (errno){
                case EMFILE:
                    #ifdef DEBUG_LOG
                    Log::db << "Accept::thListener " << strerror(EMFILE) << "\n";
                    #endif

                    #ifdef DEBUG_VERBOSE
                    cerr << "[Accept::thListener] too many connection" << endl;
                    #endif
                    close(connsd);
                    continue;
                default:
                    Log::err << "[Accept::thListener] Error in accept" << strerror(EMFILE) << "\n";
                    exit(EX_PROTOCOL);
            }
        }
        a->socketSettings(connsd);
        c = new Connection(connsd, &info, lenSockAddr);
        count++;
        a->q->pushWaitCon(c);
    }
}

void Accept::sockInit (){
    struct rlimit limit;
    if (getrlimit(RLIMIT_NOFILE, &limit))
        perror("[Accept::sockInit] Error in getrlimit:");
    
    // MAX_CON * 2, poichè ogni connessione se usa il file System apre un ulteriore FD
    limit.rlim_cur = std::min((rlim_t) MAX_CON * 2, limit.rlim_max);
    if (setrlimit(RLIMIT_NOFILE, &limit))
        perror("[Accept::sockInit] Error in setrlimit:");

    // crea il socket di ascolto del processo verso la rete
    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("[Accept::sockInit] Error in socket");
        exit(EX_PROTOCOL);
    }

    //Rende riutilizzabile la porta
    int enable = 1;
    if (setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("[Accept::sockInit] setsockopt(SO_REUSEADDR) failed");

    // Configuro i parametri con la quale deve funzionare la socket, attraverso la bind
    memset((void *) &servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta
        connessioni su una qualunque delle sue intefacce di rete */
    servaddr.sin_port = htons(SERV_PORT); /* numero di porta del server */

    // assegna l'indirizzo e la porta da far guardare alla socket
    if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0){
        perror("[Accept::sockInit] Error in bind");
        exit(EX_PROTOCOL);
    }

    // Comunico al SO la dimensione della coda di attesa che deve gestire
    if (listen(listensd, BACKLOG) < 0){
        perror("[Accept::sockInit] Error in listen");
        exit(EX_PROTOCOL);
    }
}

void Accept::socketSettings (int sock){
    #define check(expr) if (!(expr)) { perror("[Accept::socketSettings] "#expr); exit(EX_OSERR); }

    int yes = 1;
    check(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

    //TCP_NODELAY
    //              If set, disable the Nagle algorithm.  This means that segments are always sent as soon as possible,
    //              even if there is only a small amount of data.  When not set, data is buffered until there is a suf‐
    //              ficient  amount  to sendGet out, thereby avoiding the frequent sending of small packets, which results
    //              in poor utilization of the network.  This option is overridden by TCP_CORK; however,  setting  this
    //              option forces an explicit flush of pending output, even if TCP_CORK is currently set.
    //    check(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,(char *) &yes, sizeof(int)) != -1);


    //SO_RCVTIMEO and SO_SNDTIMEO
    //              Specify the receiving or sending timeouts until reporting an  error.   The  argument  is  a  struct
    //              timeval.   If an input or output function blocks for this period of time, and data has been sent or
    //              received, the return value of that function will be the amount of data transferred; if no data  has
    //              been  transferred and the timeout has been reached, then -1 is returned with errno set to EAGAIN or
    //              EWOULDBLOCK, or EINPROGRESS (for connect(2)) just as if the socket was specified to be nonblocking.
    //              If  the timeout is set to zero (the default), then the operation will never timeout.  Timeouts only
    //              have effect for  system  calls  that  perform  socket  I/O  (e.g.,  read(2),  recvmsg(2),  sendGet(2),
    //              sendmsg(2)); timeouts have no effect for select(2), poll(2), epoll_wait(2), and so on

    //    struct timeval tv;
    //    tv.tv_sec = 1;
    //    check(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) != 1);
    //    check(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv)) != 1);


    //TCP_KEEPIDLE (since Linux 2.4)
    //              The time (in seconds) the connection needs to remain idle
    //              before TCP starts sending keepalive probes, if the socket
    //              option SO_KEEPALIVE has been set on this socket.  This option
    //              should not be used in code intended to be portable.
    int idle = 1;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

    //TCP_KEEPINTVL (since Linux 2.4)
    //              The time (in seconds) between individual keepalive probes.
    //              This option should not be used in code intended to be
    //              portable.
    int interval = 5;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

    //TCP_KEEPCNT (since Linux 2.4)
    //              The maximum number of keepalive probes TCP should sendGet before
    //              dropping the connection.  This option should not be used in
    //              code intended to be portable.
    int maxpkt = 10;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);

    //TCP_NODELAY
    //              If set, disable the Nagle algorithm.  This means that segments
    //              are always sent as soon as possible, even if there is only a
    //              small amount of data.  When not set, data is buffered until
    //              there is a sufficient amount to sendGet out, thereby avoiding the
    //              frequent sending of small packets, which results in poor
    //              utilization of the network.  This option is overridden by
    //              TCP_CORK; however, setting this option forces an explicit
    //              flush of pending output, even if TCP_CORK is currently set.
    // Lo vogliamo DISATTIVATO, per aumentare l'efficenza della rete, al costo di un piccolo ritardo
    int no = 0;
    check(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &no, sizeof(int)) != -1);

    #undef check
}

