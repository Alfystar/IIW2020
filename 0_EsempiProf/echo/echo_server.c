/* Server TCP ricorsivo per il servizio di echo 
   Ultima revisione: 14 gennaio 2008 */

#include "basic.h"
#include "echo_io.h"

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
