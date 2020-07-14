/* count_clientTCP.c - code for example client program that uses TCP 
   Tratto da D.E. Comer, "Computer Networks and Internets", cap. 28 
   Ultima revisione: 14 gennaio 2008 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PROTOPORT       5193            /* default protocol port number */

/*------------------------------------------------------------------------
 * Program:   count_client
 * Purpose:   allocate a socket, connect to a server, and print all output
 * Syntax:    client [ host [port] ]
 *               host  - name of a computer on which server is executing
 *               port  - protocol port number server is using
 * Note:      Both arguments are optional.  If no host name is specified,
 *            the client uses "localhost"; if no protocol port is
 *            specified, the client uses the default given by PROTOPORT.
 *-----------------------------------------------------------------------*/

int main(int argc, char **argv)
{
  struct  hostent  *ptrh;  /* pointer to a host table entry   */
  struct  protoent *ptrp;  /* pointer to a protocol table entry   */
  struct  sockaddr_in sad; /* structure to hold an IP address     */
  int     sd;          /* socket descriptor           */
  int     port;        /* protocol port number        */
  char    *host;       /* pointer to host name        */
  int     n;           /* number of characters read       */
  char    buf[1000];       /* buffer for data from the server     */
  char    localhost[] = "localhost";    /* default host name */

  memset((void *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
  sad.sin_family = AF_INET;     /* set family to Internet     */

  /* Check command-line argument for protocol port and extract    
     port number if one is specified.  Otherwise, use the default 
     port value given by constant PROTOPORT           */

  if (argc > 2)         /* if protocol port specified */
    port = atoi(argv[2]);       /* convert to binary */
  else 
    port = PROTOPORT;       /* use default port number */
  
  if (port > 0)         /* test for legal value   */
    sad.sin_port = htons(port);
  else {            /* print error message and exit */
    fprintf(stderr,"bad port number %s\n",argv[2]);
    exit(1);
  }

  /* Check host argument and assign host name. */
  if (argc > 1) host = argv[1];   /* if host argument specified   */
  else host = localhost;

  /* Convert host name to equivalent IP address and copy to sad. */
  ptrh = gethostbyname(host);
  if (ptrh == NULL ) {
    herror("gethostbyname");
    exit(1);
  }
  memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

  /* Map TCP transport protocol name to protocol number. */
  if ( (ptrp = getprotobyname("tcp")) == NULL) {
    fprintf(stderr, "cannot map \"tcp\" to protocol number\n");
    exit(1);
  }

  /* Create a socket. */
  if ((sd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto)) < 0) {
    perror("socket creation failed");
    exit(1);
  }

  /* Connect the socket to the specified server. */
  if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
    perror("connect failed");
    exit(1);
  }

  /* Repeatedly read data from socket and write to user's screen. */
  n = recv(sd, buf, sizeof(buf), 0);
  while (n > 0) {
    write(1,buf,n);
    n = recv(sd, buf, sizeof(buf), 0);
  }

  close(sd);

  exit(0);
}
