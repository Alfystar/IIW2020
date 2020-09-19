/* Server TCP con preforking e passaggio descrittore 
   Processo padre
   Ultima revisione: 19 ottobre 2008 */

#include <signal.h>
#include <wait.h>
#include "basic.h"
#include "child_pipe.h"
#include "prefork_io.h"

static int nchildren;

pid_t child_make (int i, int listenfd, int addrlen);

int main (int argc, char **argv){
    int listensd, i, navail, maxfd, nsel, connsd, rc;
    void sig_int (int);
    pid_t child_make (int, int, int);
    ssize_t n;
    fd_set rset, masterset;
    socklen_t addrlen, clilen;
    struct sockaddr_in servaddr;
    struct sockaddr *cliaddr;

    if (argc != 2){
        fprintf(stderr, "utilizzo: prefork_pipe <#children>\n");
        exit(1);
    }

    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("errore in socket");
        exit(1);
    }

    memset((void *) &servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0){
        perror("errore in bind");
        exit(1);
    }

    if (listen(listensd, BACKLOG) < 0){
        perror("errore in listen");
        exit(1);
    }

    FD_ZERO(&masterset);
    FD_SET(listensd, &masterset);
    maxfd = listensd;
    if ((cliaddr = (struct sockaddr *) malloc(addrlen)) == NULL){
        fprintf(stderr, "errore in malloc\n");
        exit(1);
    }

    nchildren = atoi(argv[1]);
    navail = nchildren;        /* figli disponibili per servire connessioni */
    if ((cptr = (Child *) calloc(nchildren, sizeof(Child))) == NULL){
        fprintf(stderr, "errore in calloc\n");
        exit(1);
    }

    /* prefork di tutti i figli */
    for (i = 0; i < nchildren; i++){
        child_make(i, listensd, addrlen);    /* il padre ritorna */
        FD_SET(cptr[i].child_pipefd, &masterset);
        if (cptr[i].child_pipefd > maxfd)
            maxfd = cptr[i].child_pipefd;
    }

    if (signal(SIGINT, sig_int) == SIG_ERR){
        fprintf(stderr, "errore in signal\n");
        exit(1);
    }

    for (;;){
        rset = masterset;
        if (navail <= 0)
            FD_CLR(listensd, &rset);    /* listensd tolto dall'insieme se non ci sono figli disponibili */
        if ((nsel = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0){
            perror("errore in bind");
            exit(1);
        }

        /* controlla se ci sono nuove connessioni */
        if (FD_ISSET(listensd, &rset)){
            clilen = addrlen;
            if ((connsd = accept(listensd, cliaddr, &clilen)) < 0){
                perror("errore in accept");
                exit(1);
            }

            for (i = 0; i < nchildren; i++)
                if (cptr[i].child_status == 0)
                    break;    /* trovato figlio disponibile */

            if (i == nchildren){
                fprintf(stderr, "non ci sono figli disponibili\n");
                exit(1);
            }
            cptr[i].child_status = 1;    /* segna il figlio come occupato */
            cptr[i].child_count++;    /* incrementa il numero di connessioni servite dal figlio */
            navail--;            /* decrementa il numero di figli disponibili */

            /* Passaggio del socket di connessione al figlio */
            if ((n = write_fd(cptr[i].child_pipefd, "", 1, connsd)) < 0){
                fprintf(stderr, "errore in write_fd\n");
                exit(1);
            }
            if (close(connsd) == -1){
                perror("errore in close");
                exit(1);
            }
            if (--nsel == 0)
                continue;    /* finito il servizio di tutti i risultati di select() */
        }

        /* trova i figli nuovamente disponibili */
        for (i = 0; i < nchildren; i++){
            if (FD_ISSET(cptr[i].child_pipefd, &rset)){
                if ((n = read(cptr[i].child_pipefd, &rc, 1)) == 0){
                    fprintf(stderr, "child %d terminato inaspettatamente\n", i);
                    exit(1);
                }
                if (n < 0){
                    perror("errore in read");
                    exit(1);
                }
                cptr[i].child_status = 0;    /* segna il figlio come disponibile */
                navail++;            /* incrementa il numero di figli disponibili */
                if (--nsel == 0)
                    break;    /* finito il servizio di tutti i risultati di select() */
            }
        }
    }
}

void sig_int (int signo){
    int i;
    void pr_cpu_time (void);

    /* termina tutti i processi child */
    for (i = 0; i < nchildren; i++)
        kill(cptr[i].child_pid, SIGTERM);
    while (wait(NULL) > 0);    /* attende tutti i processi child */

    if (errno != ECHILD){
        fprintf(stderr, "errore in wait\n");
        exit(1);
    }

    pr_cpu_time();

    for (i = 0; i < nchildren; i++)
        printf("child %d, %ld connessioni\n", i, cptr[i].child_count);

    exit(0);
}
