//
// Created by filippob on 12/09/20.
//

#ifndef HTTP_IMAGESERVER_SHREDDER_H
#define HTTP_IMAGESERVER_SHREDDER_H

#include <cstdio>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <filesystem>

#include <sysexits.h>

#ifndef  _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <csignal>
#include <poll.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "syncUtilities.h"
// todo: move to global def
#define FILE_SIZE_LIMIT 10*1024*1024 // 10 MB of images allowed before halving
#define SLEEP_TIME 10 // sleep time between shredder activations

namespace CES {

    using namespace std;
    namespace fs = std::filesystem;


    class ImgData {
    public:
        string path;
        struct stat *buf = nullptr;

        bool operator<(const ImgData &other) const { // con questo operatore posso effettuare il sort

            if (buf->st_atim.tv_sec == other.buf->st_atim.tv_sec)
                return buf->st_atim.tv_nsec < other.buf->st_atim.tv_nsec;
            else
                return buf->st_atim.tv_sec < other.buf->st_atim.tv_sec;
        }

        explicit ImgData(const string &p); //constructor
        void removeFile();


        ~ImgData();
    };


    class Shredder { // singleton class for managing files

        static Shredder *instance;
        thread *tShr;

        int sizePipe[2];
        struct pollfd pollfd = {sizePipe[readEndPipe], POLLIN, 0};

        vector<ImgData> imgVect;
        int cacheSize;

        string cache_path = "web/cache";


    public:
        static Shredder *getInstance();

        uint_fast64_t sizeOfCache();

        void updateSizeCache(int fSize);

    private:

        explicit Shredder();

        [[noreturn]] static void threadShr(Shredder *s);

        void waitUntilFullCache();

        void fillImgVect(string &path);

        int initSizePipe();

        void initCache();

        void reduceCacheUsage();

        void emptyImgVect();
    };
}


#endif //HTTP_IMAGESERVER_SHREDDER_H
