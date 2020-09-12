//
// Created by filippob on 12/09/20.
//

#ifndef HTTP_IMAGESERVER_SHREDDER_H
#define HTTP_IMAGESERVER_SHREDDER_H

#include <cstdio>
#include <iostream>

#include <thread>
#include <chrono>

#include <algorithm>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

namespace CES {

    using namespace std;

    class ImgData {
    public:
        string path;
        struct stat *buf = (struct stat *)(malloc(sizeof(struct stat)));

        // con questo operatore posso
        bool operator< (const ImgData &other) const {

            if (buf->st_atim.tv_sec == other.buf->st_atim.tv_sec)
                return buf->st_atim.tv_nsec < other.buf->st_atim.tv_nsec;
            else
                return buf->st_atim.tv_sec < other.buf->st_atim.tv_sec;
        }

        explicit ImgData(string p);

    };


    class Shredder { // singleton class for managing files

        static Shredder *instance;
        thread *tShr;

        vector<ImgData> imgsVect;


    public:
        static Shredder &getInstance();

    private:
        explicit Shredder();

        [[noreturn]] [[noreturn]] static void threadShr(Shredder *s);

        void fillImgsVect(string &p);

    };


}


#endif //HTTP_IMAGESERVER_SHREDDER_H
