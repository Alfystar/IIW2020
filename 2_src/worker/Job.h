//
// Created by alfystar on 20/07/20.
//

#ifndef HTTP_IMAGESERVER_JOB_H
#define HTTP_IMAGESERVER_JOB_H


#include <cstdio>
#include <string>

#include <NCS.h>


namespace WORKER{
	using namespace std;

	class Job{
		NCS::Queue *q;
		thread *tJob;
		string myName;
	public:
		Job(string &name);

		static void thJob();

	private:


	};
}

#endif //HTTP_IMAGESERVER_JOB_H
