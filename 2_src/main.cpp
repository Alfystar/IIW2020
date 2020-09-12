//
// Created by alfystar on 14/07/20.
//


#include <unistd.h>
#include <sys/sysinfo.h>


#include <CES.h>
#include <Log.h>

#define nWorker get_nprocs()*2  //Per avere il doppio dei core e gestire le situazioni di wait forzate

using namespace std;

int main(__attribute__((unused))int argc, __attribute__((unused))char *argv[]){
	Log::initLogger();
	CES::initCES(nWorker);
	while(true);
//	sleep(60);
//	return 0;
}
