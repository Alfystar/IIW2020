//
// Created by alfystar on 20/07/20.
//

#include "CES.h"

using namespace CES;

int CES::nWorkers = 0;

Shredder &shr = Shredder::getInstance();

Worker **CES::workers = nullptr; //worker per la gestione delle richieste

void CES::initCES(int n){
	cout << "CES::initCES CES subSystem preAlloc " << n << " workers\n";
	//todo impostare PWD alla directory passata da argv
	CES::nWorkers = n;
	workers = (Worker **)calloc(n, sizeof(Worker **));
	string name;

	for(int i = 0; i < n; ++i){
		name = fmt::format("Worker {}", i);
		workers[i] = new Worker(name);
	}


}