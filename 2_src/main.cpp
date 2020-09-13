//
// Created by alfystar on 14/07/20.
//


#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <CES.h>
#include <Log.h>

#define nWorker get_nprocs()*2  //Per avere il doppio dei core e gestire le situazioni di wait forzate

using namespace std;

int main(__attribute__((unused))int argc, __attribute__((unused))char *argv[]){
	Log::initLogger();
	CES::initCES(nWorker);
//	int ret, sigCode;
//	sigset_t set;
	while(true){
		pause();
//		ret = sigwait(&set, &sigCode);
		//Gestione dei segnali
	};
	return 0;
}
