#include "basic.h"
#include "prefork_io.h"

#define    MAXN    16384        /* massimo numero di byte che un client puo' richiedere */

void web_child(int sockfd) {
    int ntowrite;
    ssize_t nread;
    char line[MAXLINE], result[MAXN];

    for (;;) {
        /* legge la richiesta inviata dal client (contenuta in una sola linea) */
        if ((nread = readline(sockfd, line, MAXLINE)) == 0)
            return;        /* connessione chiusa dal client */

        /* la linea di richiesta del client specifica il numero di byte da fornire in risposta */
        ntowrite = atol(line);
        if ((ntowrite <= 0) || (ntowrite > MAXN)) {
            fprintf(stderr, "client request for %d bytes", ntowrite);
            exit(1);
        }
        writen(sockfd, result, ntowrite);    /* invia la risposta */
    }
}
