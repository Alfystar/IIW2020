/* Server TCP ricorsivo per il servizio di echo 
   con gestione del segnale SIGCHILD 
   Ultima revisione: 14 gennaio 2008 */

#include "basic.h"
#include "echo_io.h"

/* Gestione segnale SIGCHLD */
#include <signal.h>
#include <sys/wait.h>

typedef void Sigfunc (int);
Sigfunc *signal (int signum, Sigfunc *handler);
void sig_chld_handler (int signum);
/***/

void str_srv_echo (int sockd);

int main (int argc, char **argv){
    pid_t pid;
    int listensd, connsd;
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len;

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

    /* Alla ricezione del segnale SIGCHLD segue il comportamento
       definito da sig_chld_handler */
    if (signal(SIGCHLD, sig_chld_handler) == SIG_ERR){
        fprintf(stderr, "errore in signal");
        exit(1);
    }

    for (;;){
        len = sizeof(cliaddr);
        if ((connsd = accept(listensd, (struct sockaddr *) &cliaddr, &len)) < 0){
            perror("errore in accept");
            exit(1);
        }

        if ((pid = fork()) == 0){
            if (close(listensd) == -1){
                perror("errore in close");
                exit(1);
            }
            printf("%s:%d connesso\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            printf("Pid del processo figlio %d\n", (int) getpid());
            str_srv_echo(connsd);    /* svolge il lavoro del server */

            if (close(connsd) == -1){
                perror("errore in close");
                exit(1);
            }
            exit(0);
        } /* end fork */

        if (close(connsd) == -1){    /* processo padre */
            perror("errore in close");
            exit(1);
        }
    } /* end for */
}

/******/
void str_srv_echo (int sockd){
    int n;
    char line[MAXLINE];

    for (;;){
        if ((n = readline(sockd, line, MAXLINE)) == 0)
            return; /* il client ha chiuso la connessione e inviato EOF */

        if (writen(sockd, line, n) != n){
            fprintf(stderr, "errore in write");
            exit(1);
        }
    }
}

/******/
Sigfunc *signal (int signum, Sigfunc *func){
    struct sigaction act, oact;
    /* la struttura sigaction memorizza informazioni riguardanti la
  manipolazione del segnale */

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);    /* non occorre bloccare nessun altro segnale */
    act.sa_flags = 0;
    if (signum != SIGALRM)
        act.sa_flags |= SA_RESTART;
    if (sigaction(signum, &act, &oact) < 0)
        return (SIG_ERR);
    return (oact.sa_handler);
}

/******/
void sig_chld_handler (int signum){
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        printf("child %d terminato\n", pid);

    return;
}
