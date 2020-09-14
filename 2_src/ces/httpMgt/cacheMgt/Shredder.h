//
// Created by filippob on 12/09/20.
//

#ifndef HTTP_IMAGESERVER_SHREDDER_H
#define HTTP_IMAGESERVER_SHREDDER_H

#include <cstdio>

#include  <shared_mutex>

#include <thread>
#include <chrono>

#include <algorithm>
#include <vector>

#include <fstream>
#include <iostream>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>


#include <pthread.h>

#include "syncUtilities.h"

#define FILE_SIZE_LIMIT 10*1024*1024 // 10 MB of images allowed before halving
#define SLEEP_TIME 10 // sleep time between shredder activations

namespace CES {

    using namespace std;
    namespace fs = std::filesystem;


    class ImgData {
    public:
        string path;
        struct stat *buf = nullptr;

        bool operator< (const ImgData &other) const { // con questo operatore posso effettuare il sort

            if (buf->st_atim.tv_sec == other.buf->st_atim.tv_sec)
                return buf->st_atim.tv_nsec < other.buf->st_atim.tv_nsec;
            else
                return buf->st_atim.tv_sec < other.buf->st_atim.tv_sec;
        }

        explicit ImgData(string p); //constructor
        void removeFile();


        ~ImgData();
    };


    class Shredder { // singleton class for managing files

        static Shredder *instance;
        thread *tShr;

        vector<ImgData> imgsVect;


    public:
        static Shredder &getInstance();

        uint_fast64_t sizeOfCache();

    private:
        explicit Shredder();

        [[noreturn]] [[noreturn]] static void threadShr(Shredder *s);

        void fillImgsVect(string &path);
        void reduceCacheUsage();
        void emptyCache();

    };


}


#endif //HTTP_IMAGESERVER_SHREDDER_H
