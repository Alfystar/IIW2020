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

#include <Log.h>

#include <sysexits.h>

#ifndef  _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <csignal>
#include <poll.h>

#include <fmt/format.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "SyncUtilities.h"
#include "timeSpecOp.h"
#include "../../../GLOBAL_DEF.h"

#ifndef CACHE_PATH
#define CACHE_PATH "./web/cache"
#endif

#ifndef FILE_SIZE_LIMIT
#define FILE_SIZE_LIMIT 10*1024*1024 // 10 MB of images allowed before halving
#endif

namespace CES {

    using namespace std;
    namespace fs = std::filesystem;

    class Shredder{ // singleton class for managing files

    public:
        class ImgData{
        public:
            string path;
            struct stat *statFile = nullptr;

            bool operator< (const ImgData &other) const{ // con questo operatore posso effettuare il sort
                // usiamo ">" così da ordinare dal più remoto al più recente
                return timercmpSpec(&statFile->st_atim, &other.statFile->st_atim, >);
            }

            explicit ImgData (const string &p); //constructor
            int removeFile ();
            ~ImgData ();
        };

    private:
        static Shredder *instance;
        thread *tShr;
        int sizePipe[2] = {0, 0};
        struct pollfd pollFd = {0, 0, 0};
        vector <ImgData> imgVect;
        uint_fast64_t cacheSize = 0;
        string cache_path = CACHE_PATH;
        int countFile = 0;

    public:
        static Shredder *getInstance ();
        uint_fast64_t sizeOfCache ();
        void updateSizeCache (uint_fast64_t fSize);

    private:
        explicit Shredder ();
        [[noreturn]] static void threadShr (Shredder *s);
        void freeSpace ();
        void waitUntilFullCache ();
        void elaboratePipe ();
        void fillImgVect (string &path);
        inline int initSizePipe ();
        void initCache ();
        void reduceCacheUsage ();
        inline void emptyImgVect ();
        static string sizeFormatted (uint_fast64_t size);
    };
}

#endif //HTTP_IMAGESERVER_SHREDDER_H
