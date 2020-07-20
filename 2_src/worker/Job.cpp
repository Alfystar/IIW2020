//
// Created by alfystar on 20/07/20.
//

#include "Job.h"

using namespace WORKER;

Job::Job(string &name){
	q = NCS::ncsGetQueue();
	myName = name;

}

void Job::thJob(){

}
