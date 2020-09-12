//
// Created by alfystar on 20/07/20.
//

#ifndef HTTP_IMAGESERVER_CES_H
#define HTTP_IMAGESERVER_CES_H


#include <fmt/format.h>


#include "Worker.h"

#include "httpMgt/cacheMgt/Shredder.h"

namespace CES{

    extern Shredder shredder;

	extern int nWorkers;
	extern Worker **workers;  // teniamo traccia dei thread job

	void initCES(int n);
}


#endif //HTTP_IMAGESERVER_CES_H
