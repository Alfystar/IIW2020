//
// Created by alfystar on 22/07/20.
//

#ifndef HTTP_IMAGESERVER_LOG_H
#define HTTP_IMAGESERVER_LOG_H


#include <unistd.h>
#include <time.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <thread>


namespace Log {
    using namespace std;

    extern ostream out;        /// Linked to standard output file
    extern ostream err;        /// Linked to standard error file
    extern ostream db;        /// Linked to standard error file
    extern fstream *outfile;
    extern fstream *errfile;
    extern fstream *dbfile;
    extern thread *refreshThread;

    void initLogger();

    [[noreturn]] void syncStream();
}


#endif //HTTP_IMAGESERVER_LOG_H
