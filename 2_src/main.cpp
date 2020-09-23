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

// Init di default delle variabili
volatile int SERV_PORT = 8080;
volatile int nWorker = get_nprocs();

using namespace std;

#ifdef EXE_NAME_STRING
#define NAME EXE_NAME_STRING
#else
#define NAME "badAlphaWeb"
#endif

void help (){
    cout << "Server web badAlphaWeb, versione 0.1\n";
    cout << "Per eseguire correttamente il programma digitare:\n";
    cout << "\t 1)  ./" << NAME << " <PORT> [OPTION]                := Home server al PWD corrente\n";
    cout << "\t 2)  ./" << NAME << " <PORT> <serverHome> [opzioni]  := Home server spostata alla dir desiderata\n";
    cout << "\t 3)  ./" << NAME << " -h  o  badAlpha --help         := Per leggere questa guida\n";
    cout << "OPTION List: (In caso di ripetizioni, l'ultimo ha effetto)\n";
    cout << "\t -w <NumberOfWorker>\t:= numero di worker instanziati dal server, di default = 8 and MUST be >=0\n";
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
    //todo: Verificare che al ctrl+c, a terminare sia il figlio e non il padre
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
                    case SIGINT:
                        cerr << "[MAIN::DAD] caused by the SIGINT";
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

// setta il primo parametro che incontra, richiamare avanzando gli indici
// Ritorna di quanti indici avanzare
int paramParse (int argc, char *argv[]){
    if (argc >= 1){
        // Opzione -w <Num Worker>
        if (strcmp(argv[0], "-w") == 0){
            if (argc > 1){
                int val;
                if (sscanf(argv[1], "%d", &val) > 0){
                    if (val > 0){
                        nWorker = val; // it's an integer
                        return 2;   // Ho usato 2 argomenti
                    }
                }
            }
            return -1;  // Se arrivo qui allora il parametro aveva unErrore
        }
    }

    return -1;  // errore
}

void systemSetup (int argc, char *argv[]){
    if (argc < 2){
        help();
        exit(EXIT_FAILURE);
    }

    /// Parametri obbligatori
    if (argc > 1){
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
    if (argc > 2){
        if (initStorage(argv[2])){
            cout << "[MAIN::main] Direcotry change fail\n";
            exit(EX_OSFILE);
        }
    }
    else{
        cout << "[MAIN::main] Direcotry change success\n";
        cout << "To have help use option -h or --help\n";
    }

    /// Scan dei parametri facoltativi
    int argcScan = 3;
    int forward;
    while (argcScan < argc){
        forward = paramParse(argc - argcScan, argv + argcScan);
        if (forward == -1){
            cout << "[MAIN::main] At the param: \"" << argv[argcScan] << "\" had use the wrong syntax" << endl;
            help();
            exit(EX_CONFIG);
        }
        argcScan += forward;
    }
}

int main (int argc, char *argv[]){
    systemSetup(argc, argv);

    sleep(1);
    #ifdef DAD_RECREATE
    dadCreator();
    #else
    sonServer();
    #endif
    return 0;
}