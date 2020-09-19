//
// Created by alfystar on 22/07/20.
//

#include "Log.h"

using namespace Log;

ostream Log::out(nullptr);        /// Linked to standard output file
ostream Log::err(nullptr);        /// Linked to standard error file
ostream Log::db(nullptr);        /// Linked to standard error file

fstream *Log::outfile;
fstream *Log::errfile;
fstream *Log::dbfile;
thread *Log::refreshThread = nullptr;

namespace fs = std::filesystem;

void Log::initLogger (){
    fs::create_directories("./Debug/");
    if (fs::exists("./logDb.txt")){
        struct stat statbuf;
        stat("./logDb.txt", &statbuf);

        struct tm *nowtm;
        char tmbuf[64];
        nowtm = localtime(&statbuf.st_ctime);
        strftime(tmbuf, sizeof(tmbuf), "%F__%T", nowtm);

        char newDebugName[1024];
        sprintf(newDebugName, "./Debug/%s__logDb.txt", tmbuf);
        fs::rename("./logDb.txt", newDebugName);
    }

    time_t rawtime;
    time(&rawtime);
    char *timeStr = ctime(&rawtime);

    Log::outfile = new fstream("logOut.txt", ios::app);
    Log::errfile = new fstream("logErr.txt", ios::app);
    Log::dbfile = new fstream("logDb.txt", ios::app);

    Log::out.rdbuf(outfile->rdbuf());
    Log::err.rdbuf(errfile->rdbuf());
    Log::db.rdbuf(dbfile->rdbuf());

    out << "Output Log restart at: " << timeStr << endl;
    err << "Output Log restart at: " << timeStr << endl;
    db << "Output Log restart at: " << timeStr << endl;

    Log::refreshThread = new thread(syncStream);
}

[[noreturn]] void Log::syncStream (){
    pthread_setname_np(pthread_self(), "Logger");

    while (true){
        Log::out << flush;
        Log::err << flush;
        Log::db << flush;
        sleep(1);  // Sveglio ogni secondo
    }
}
