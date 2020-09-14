//
// Created by filippob on 11/09/20.
//

#ifndef HTTP_IMAGESERVER_RESOURCE_H
#define HTTP_IMAGESERVER_RESOURCE_H

#include <cstdio>
#include <iostream>

#include <string>
#include <math.h>


// for open() and close()
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "syncUtilities.h"


namespace CES {

    using namespace std;

    class Resource {
        string path;
        int fd;

    public:
        explicit Resource(string &path, float &qValue);

        ~Resource();


        string &getPath();

    private:

        void elaborateFile(string &file, string &scale);

    };

}


#endif //HTTP_IMAGESERVER_RESOURCE_H
