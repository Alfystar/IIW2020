//
// Created by alfystar on 14/07/20.
//

#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <CES.h>
#include <Log.h>
#include "GLOBAL_DEF.h"


volatile int SERV_PORT = 8080;

using namespace std;

void help (){
    cout << "Server web badAlphaWeb, versione 0.1\n";
    cout << "Per eseguire correttamente il programma digitare:\n";
    cout << "\t 1) >> badAlphaWeb <PORT_Number>                         := Home server al PWD corrente\n";
    cout << "\t 2) >> badAlphaWeb <PORT_Number> <webServer home Path>   := Home server spostata alla dir desiderata\n";
    cout << "\t 3) >> badAlphaWeb -h  o  badAlpha --help                := Per leggere questa guida\n";
    cout << "";
}

int initStorage (char *storage)  //apre o crea un nuovo storage per il server, la struttura e a cura del sito
{
    /* modifica il path reference dell'env per "spostare" il programma nella nuova locazione
     * la variabile PWD contiene il path assoluto, della working directory, ed è aggiornata da una sheel
     * MA I PROGRAMMI SCRITTI IN C usano un altra variabile per dire il proprio percorso di esecuzione.
     * Di conseguenza bisogna prima modificare il path del processo e sucessivamente aggiornare l'env
     */

    printf("[1]---> Fase 1, opening of storage\n");

    int errorRet;
    errorRet = chdir(storage);                        //modifico l'attuale directory di lavoro del processo
    if (errorRet != 0)    //un qualche errore nel ragiungimento della cartella
    {
        switch (errno){
            case 2: //No such file or directory
                printf("[MAIN::initStorage] directory not exists, now proceed to creation\n");
                errorRet = mkdir(storage, 0777);
                if (errorRet == -1){
                    perror("[MAIN::initStorage] mkdir() fails for");
                    return -1;
                }
                else{
                    printf("[MAIN::initStorage] New directory create\n");
                    errorRet = chdir(storage);
                    if (errorRet == -1){
                        perror("[MAIN::initStorage] chdir() fail for");
                        return -1;
                    }
                }
                break;
            default:
                perror("[MAIN::initStorage] chdir() fail for");
                return -1;
        }
    }
    char curDirPath[100];
    errorRet = setenv("PWD", getcwd(curDirPath, 100), true);    //aggiorno l'env per il nuovo pwd
    if (errorRet != 0) perror("[MAIN::initStorage] setEnv('PWD') fail for");
    printf("[MAIN::initStorage] Current Directory set:\n-->\tgetcwd()=%s\n-->\tPWD=%s\n", curDirPath, getenv("PWD"));
    printf("[1]---> Success\n\n");

    return 0;   //directory cambiata con successo
}

[[noreturn]]void sonServer (){
    Log::initLogger();
    CES::initCES(nWorker);

    while (true){
        pause();
    };
}

void dadCreator (){
    while (true){
        pid_t pid = fork();
        if (pid == -1){
            perror("MAIN:: fork error");
            exit(EX_OSERR);
        }
        if (pid == 0){ //son
            cout << "[MAIN::SON] start new instance" << endl;
            sonServer();
        }
        else{        //dad
            int wstatus;
            int ret = wait(&wstatus);
            if (ret == -1){
                perror("[MAIN::DAD] wait error");
                exit(EX_OSERR);
            }
            if (WIFEXITED(wstatus)){     // good end
                exit(EX_OK);
            }
            cerr << "[MAIN::DAD] Son exit with error code: " << wstatus << endl;
            if (WIFSIGNALED(wstatus)){   // signal un-catch
                int sNum = WTERMSIG(wstatus);
                switch (sNum){
                    case SIGKILL:
                        cerr << "[MAIN::DAD] caused by the SIGKILL";
                        exit(EX_OK);
                        break;
                    case SIGTERM:
                        cerr << "[MAIN::DAD] caused by the SIGTERM";
                        exit(EX_OK);
                        break;
                    default:
                        cerr << "[MAIN::DAD] caused by the signal n°" << sNum << " means:" << strsignal(sNum) << endl;
                        break;
                }
            }
        }
    }
}

int main (int argc, char *argv[]){
    if (argc >= 1){
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
            help();
            exit(EX_OK);
        }
        else{
            SERV_PORT = atoi(argv[1]);
            if (SERV_PORT == 0){ // sia che sia 0 la porta passato o un errore di digitazione
                cout << "[MAIN::main] SERV_PORT passed is invalid, please use NUMBER and different from 0\n";
                exit(EX_DATAERR);
            }
        }
    }
    if (argc >= 2){

        if (initStorage(argv[2])){
            cout << "[MAIN::main] Direcotry change fail\n";
            exit(EX_OSFILE);
        }
    }
    else{
        cout << "[MAIN::main] Direcotry change success\n";
        cout << "To have help use option -h or --help\n";
    }
    sleep(1);
    #ifdef DAD_RECREATE
    dadCreator();
    #else
    sonServer();
    #endif
    return 0;
}