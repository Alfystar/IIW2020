//
// Created by alfystar on 22/07/20.
//

#include "Log.h"

using namespace Log;
ostream Log::out(nullptr);        /// Linked to standard output file
ostream Log::err(nullptr);        /// Linked to standard error file
ostream Log::db(nullptr);        /// Linked to standard error file

fstream Log::outfile("logOut.txt", ios::app);
fstream Log::errfile("logErr.txt", ios::app);
fstream Log::dbfile("logDb.txt", ios::app);

thread *Log::refreshThread = nullptr;

void Log::initLogger(){

	time_t rawtime;
	time(&rawtime);
	char *timeStr = ctime(&rawtime);

	Log::out.rdbuf(outfile.rdbuf());
	Log::err.rdbuf(errfile.rdbuf());
	Log::db.rdbuf(dbfile.rdbuf());

	out << "Output Log restart at: " << timeStr << endl;
	err << "Output Log restart at: " << timeStr << endl;
	db << "Output Log restart at: " << timeStr << endl;

	Log::refreshThread = new thread(syncStream);
}

[[noreturn]] void Log::syncStream(){
	while(true){
		Log::out << flush;
		Log::err << flush;
		Log::db << flush;
		sleep(5);  // Sveglio 4 volte al secondo
	}

}
