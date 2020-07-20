//
// Created by alfystar on 20/07/20.
//

#ifndef HTTP_IMAGESERVER_WORKER_H
#define HTTP_IMAGESERVER_WORKER_H


#include "Job.h"

namespace WORKER{

	int nJobs;
	Job *jobs;  // teniamo traccia dei thread job
	void initWorker(int n);
}


#endif //HTTP_IMAGESERVER_WORKER_H
