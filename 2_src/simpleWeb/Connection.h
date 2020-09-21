//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_CONNECTION_H
#define HTTP_IMAGESERVER_CONNECTION_H

#include <cstdio>
#include <cstring>
#include <atomic>
#include <sstream>
#include <fstream>
#include <fmt/format.h>

#include <poll.h>
#include <sys/socket.h>

#include <Utility.hpp>

#include <Log.h>

#define MAXLINE 4096

namespace NCS {
    using namespace std;

    class Connection{
    public:
        class httpHeader{
        public:
            std::string method;
            std::string path;
            std::string query_string;
            std::string version;
            SimpleWeb::CaseInsensitiveMultimap cim;

            httpHeader (){}

            ~httpHeader (){}
        };

        enum ConnectType{
            internalConnect, httpConnect, tcpConnect, unknown
        };
        int fd;
        struct sockaddr sockInfo;
        socklen_t socklen;

    private:
        ConnectType cType = unknown;
//        static std::atomic <unsigned long> count;

    public:
        Connection (int fd);
        Connection (int fd, struct sockaddr *sockInfo, socklen_t socklen);
        ~Connection ();
//        unsigned long activeConnection ();
        void compilePollFD (struct pollfd *pollFd);
        ConnectType getType ();
        int sendData (const void *data, int datalen);    // 0 = send succes, -1 = error, look errno
        int sendStr (const std::string &s);
        httpHeader *readHttpHeader ();
    };
}

#endif //HTTP_IMAGESERVER_CONNECTION_H
