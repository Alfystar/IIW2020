//
// Created by alfystar on 20/07/20.
//

#ifndef HTTP_IMAGESERVER_WORKER_H
#define HTTP_IMAGESERVER_WORKER_H


#include <cstdio>
#include <string>


#include <NCS.h>
#include <unistd.h>

#include <utility.hpp>
#include <Connection.h>

#include <Log.h>

namespace CES{
	using namespace std;

	class Worker{
		NCS::Queue *q;
		thread *tJob;
		string myName;
	public:
		Worker(string &name);

		static void thWorker(Worker *j);

	private:


	};
}

#endif //HTTP_IMAGESERVER_WORKER_H