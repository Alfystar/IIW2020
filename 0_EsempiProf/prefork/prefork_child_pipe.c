/* Server TCP con preforking e passaggio descrittore 
   Preforking e processo figlio
   Ultima revisione: 19 ottobre 2008 */

#include "basic.h"
#include "prefork_io.h"
#include "child_pipe.h"

pid_t child_make (int i, int listensd, int addrlen){
    int sockfd[2];    /* coppia di socket locali per la comunicazione tra padre e figlio */
    pid_t pid;
    void child_main (int, int, int);

    /* crea la coppia di socket locali per la comunicazione tra padre e figlio */
    if ((socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd)) < 0){
        perror("errore in socketpair");
        exit(1);
    }

    if ((pid = fork()) > 0){    /* processo padre */
        if (close(sockfd[1]) == -1){
            perror("errore in close");
            exit(1);
        }
        cptr[i].child_pid = pid;
        cptr[i].child_pipefd = sockfd[0];
        cptr[i].child_status = 0;    /* segna il figlio come disponibile */
        return (pid);        /* il padre ritorna */
    }

    /* copia dello stream pipe */
    if ((dup2(sockfd[1], STDERR_FILENO)) < 0){
        perror("errore in dup2");
        exit(1);
    }
    if (close(sockfd[0]) == -1){
        perror("errore in close");
        exit(1);
    }
    if (close(sockfd[1]) == -1){
        perror("errore in close");
        exit(1);
    }
    if (close(listensd) == -1){
        perror("errore in close");
        exit(1);
    }
    child_main(i, listensd, addrlen);    /* non ritorna mai */
}

void child_main (int i, int listensd, int addrlen){
    char c;
    int connsd;
    void web_child (int);
    ssize_t n;

    printf("child %ld inizia\n", (long) getpid());
    for (;;){
        /* riceve il socket di connessione */
        if ((n = read_fd(STDERR_FILENO, &c, 1, &connsd)) == 0){
            fprintf(stderr, "read_fd ha restituito 0\n");
            exit(1);
        }
        if (connsd < 0){
            fprintf(stderr, "nessun descrittore da read_fd\n");
            exit(1);
        }
        web_child(connsd);        /* processa la richiesta */
        if (close(connsd) == -1){    /* chiude il socket di connessione */
            perror("errore in close");
            exit(1);
        }

        /* avvisa il padre di essere nuovamente pronto */
        if (write(STDERR_FILENO, "", 1) < 0){
            perror("errore in write");
            exit(1);
        }
    }
}
