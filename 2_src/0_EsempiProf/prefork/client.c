#include <sys/wait.h>
#include "basic.h"
#include "prefork_io.h"

#define	MAXN	16384     /* max # byte da richiedere al server */

int main(int argc, char **argv)
{
  int     i, j, sockfd, nchildren, nloops, nbytes;
  pid_t   pid;
  ssize_t n;
  char    request[MAXLINE], reply[MAXN];
  struct  sockaddr_in servaddr;

  if (argc != 5) {
    fprintf(stderr, "utilizzo: client <indirizzo IP server> <#children> "
                     "<#cicli/child> <#byte/richiesta>");
    exit(1);
  }    

  nchildren = atoi(argv[2]);	/* numero di figli del client */
  nloops = atoi(argv[3]);	/* numero di connessioni per figlio del client */
  nbytes = atoi(argv[4]);	/* numero di byte richiesti al server */
  snprintf(request, sizeof(request), "%d\n", nbytes); /* newline alla fine della richiesta */

  for (i=0; i<nchildren; i++) {
    if ((pid = fork()) == 0) {	/* processo figlio */
      for (j=0; j<nloops; j++) {	/* ciclo di nloops connessioni */

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
          perror("errore in socket");
          exit(1);
        }    

        memset((void *)&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
          fprintf(stderr, "errore in inet_pton per %s\n", argv[1]);
          exit(1);
        }    
	/* si connette al server */
        if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
          perror("errore in connect");
          exit(1);
        }    
	/* invia la linea di richiesta */
        writen(sockfd, request, strlen(request));
        /* legge la risposta */    
        if ((n = readn(sockfd, reply, nbytes)) != nbytes) {
          fprintf(stderr, "server risposto con %d byte\n", n);
          exit(1);
        }    
	/* chiude la connessione con il server */
        if (close(sockfd) == -1) {  /* TIME_WAIT sul client, non sul server */
          perror("errore in close");
          exit(1);
        }      
      }  /* end for */

      printf("child %d terminato\n", i);
      exit(0);
    }
    /* il processo padre cicla per effettuare un nuovo fork() */
  }

  while (wait(NULL) > 0);     /* il processo padre attende tutti i processi figli */

  if (errno != ECHILD) {
    perror("errore in wait");
    exit(1);
  }    

  exit(0);
}
