/* Client TCP per il servizio di echo */

#include "basic.h"
#include "echo_io.h"

void str_cli_echo (FILE *fd, int sockd);
ssize_t writen (int fd, const void *buf, size_t n);
int readline (int fd, void *vptr, int maxlen);

int main (int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2){
        fprintf(stderr, "utilizzo: echo_client <indirizzo IP server>\n");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("errore in socket");
        exit(1);
    }

    memset((void *) &servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
        fprintf(stderr, "errore in inet_pton per %s", argv[1]);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("errore in connect");
        exit(1);
    }

    str_cli_echo(stdin, sockfd);    /* svolge il lavoro del client */

    close(sockfd);

    exit(0);
}

/******/
void str_cli_echo (FILE *fd, int sockd){
    char sendline[MAXLINE], recvline[MAXLINE];
    int n;

    while (fgets(sendline, MAXLINE, fd) != NULL){
        if ((n = writen(sockd, sendline, strlen(sendline))) < 0){
            fprintf(stderr, "errore in write");
            exit(1);
        }

        if ((n = readline(sockd, recvline, MAXLINE)) < 0){
            fprintf(stderr, "errore in readline");
            exit(1);
        }

        fputs(recvline, stdout);
    }
}
