/* Server iterativo dell'applicazione echo che utilizza la select 
   Ultima revisione: 14 gennaio 2008 */

#include "basic.h"
#include "echo_io.h"

int main (int argc, char **argv){
    int listensd, connsd, socksd;
    int i, maxi, maxd;
    int ready, client[FD_SETSIZE];
    char buff[MAXLINE];
    fd_set rset, allset;
    ssize_t n;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

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

    /* Inizializza il numero di descrittori */
    maxd = listensd;
    maxi = -1;
    /* Inizializza l'array di interi client contenente i descrittori utilizzati */
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;

    FD_ZERO(&allset); /* Inizializza a zero l'insieme dei descrittori */
    FD_SET(listensd, &allset); /* Inserisce il descrittore di ascolto */

    for (;;){
        rset = allset;  /* Setta il set di descrittori per la lettura */
        /* ready � il numero di descrittori pronti */
        if ((ready = select(maxd + 1, &rset, NULL, NULL, NULL)) < 0){
            perror("errore in select");
            exit(1);
        }

        /* Se � arrivata una richiesta di connessione, il socket di ascolto
           � leggibile: viene invocata accept() e creato un socket di connessione */
        if (FD_ISSET(listensd, &rset)){
            len = sizeof(cliaddr);
            if ((connsd = accept(listensd, (struct sockaddr *) &cliaddr, &len)) < 0){
                perror("errore in accept");
                exit(1);
            }

            /* Inserisce il descrittore del nuovo socket nel primo posto
               libero di client */
            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i] < 0){
                    client[i] = connsd;
                    break;
                }
            /* Se non ci sono posti liberi in client, errore */
            if (i == FD_SETSIZE){
                fprintf(stderr, "errore in accept");
                exit(1);
            }
            /* Altrimenti inserisce connsd tra i descrittori da controllare
               ed aggiorna maxd */
            FD_SET(connsd, &allset);
            if (connsd > maxd) maxd = connsd;
            if (i > maxi) maxi = i;
            if (--ready <= 0) /* Cicla finch� ci sono ancora descrittori
                           leggibili da controllare */
                continue;
        }

        /* Controlla i socket attivi per controllare se sono leggibili */
        for (i = 0; i <= maxi; i++){
            if ((socksd = client[i]) < 0)
                /* se il descrittore non � stato selezionato viene saltato */
                continue;

            if (FD_ISSET(socksd, &rset)){
                /* Se socksd � leggibile, invoca la readline */
                if ((n = readline(socksd, buff, MAXLINE)) == 0){
                    /* Se legge EOF, chiude il descrittore di connessione */
                    if (close(socksd) == -1){
                        perror("errore in close");
                        exit(1);
                    }
                    /* Rimuove socksd dalla lista dei socket da controllare */
                    FD_CLR(socksd, &allset);
                    /* Cancella socksd da client */
                    client[i] = -1;
                }
                else /* echo */
                if (writen(socksd, buff, n) < 0){
                    fprintf(stderr, "errore in write");
                    exit(1);
                }
                if (--ready <= 0) break;
            }
        }
    }
}
