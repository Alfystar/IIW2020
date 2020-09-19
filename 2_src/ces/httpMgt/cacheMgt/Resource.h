//
// Created by filippob on 11/09/20.
//

#ifndef HTTP_IMAGESERVER_RESOURCE_H
#define HTTP_IMAGESERVER_RESOURCE_H

#include <iostream>
#include <string>
#include <cmath>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <Log.h>

#include "SyncUtilities.h"
#include "Shredder.h"
#include "../../../GLOBAL_DEF.h"

#ifndef CACHE_PATH
#define CACHE_PATH "./web/cache"
#endif
namespace CES {
    using namespace std;
    class Resource{
        string path;
        int fd;

    public:
        Resource (string &path, string &format, float qValue);
        Resource (string &path, float qValue);
        ~Resource ();
        string &getPath ();

    private:
        void elaborateFile (string &file, string &scale);
        void executeCommand (string &command);
    };
}

#endif //HTTP_IMAGESERVER_RESOURCE_H