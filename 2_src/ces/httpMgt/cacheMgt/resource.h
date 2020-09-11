//
// Created by filippob on 11/09/20.
//

#ifndef HTTP_IMAGESERVER_RESOURCE_H
#define HTTP_IMAGESERVER_RESOURCE_H

#include <cstdio>
#include <iostream>

#include <string>
#include <math.h>


// for open() and flock()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace CES {

    using namespace std;

    class Resource {
        const char * path = nullptr;
        int fd = 0;

    public:
        explicit Resource(float &qValue);
        ~Resource();


        basic_string<char> getPath();

    };

}


#endif //HTTP_IMAGESERVER_RESOURCE_H
