#include "basic.h"

/******/
ssize_t readn(int fd, void *buf, size_t n)
{
  size_t  nleft;
  ssize_t nread;
  char *ptr;

  ptr = buf;
  nleft = n;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else
        return(-1);
    } 
    else if (nread == 0)
      break;	/* EOF */

    nleft -= nread;
    ptr += nread;
  }
  return(n-nleft);	/* restituisce >= 0 */
}

/******/
ssize_t writen(int fd, const void *buf, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = buf;
  nleft = n;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
       if ((nwritten < 0) && (errno == EINTR)) nwritten = 0; 	    
       else return(-1);	    /* errore */
    }
    nleft -= nwritten;
    ptr += nwritten;  
  }
  return(n-nleft);	/* restituisce >= 0 */
}

/******/
int readline(int fd, void *buf, int maxlen)
{
  int  n, rc;
  char c, *ptr;

  ptr = buf;
  for (n = 1; n < maxlen; n++) {
    if ((rc = read(fd, &c, 1)) == 1) { 
      *ptr++ = c;
      if (c == '\n') break;	/* letto newline */
   } 
   else 
      if (rc == 0) {		/* read ha letto l'EOF */
 	 if (n == 1) return(0);	/* esce senza aver letto nulla */
 	 else break;
      } 
      else return(-1);		/* errore */
  }

  *ptr = 0;	/* per indicare la fine dell'input */
  return(n);	/* restituisce il numero di byte letti */
}

/******/
ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
  struct msghdr	msg;
  struct iovec	iov[1];
  union {
    struct cmsghdr	cm;
    char		control[CMSG_SPACE(sizeof(int))];
  } control_un;
  struct cmsghdr *cmptr;

  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);

  cmptr = CMSG_FIRSTHDR(&msg);
  cmptr->cmsg_len = CMSG_LEN(sizeof(int));
  cmptr->cmsg_level = SOL_SOCKET;
  cmptr->cmsg_type = SCM_RIGHTS;
  *((int *) CMSG_DATA(cmptr)) = sendfd;

  msg.msg_name = NULL;	/* destinatario del messaggio */
  msg.msg_namelen = 0;

  iov[0].iov_base = ptr;
  iov[0].iov_len = nbytes;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  return(sendmsg(fd, &msg, 0));	/* invia il messaggio al socket */
}

/******/
ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
  struct msghdr	msg;
  struct iovec	iov[1];
  ssize_t	n;
  union {
    struct cmsghdr	cm;
    char  control[CMSG_SPACE(sizeof(int))];
  } control_un;
  struct cmsghdr	*cmptr;

  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);

  msg.msg_name = NULL;	/* mittente del messaggio */
  msg.msg_namelen = 0;

  iov[0].iov_base = ptr;
  iov[0].iov_len = nbytes;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

  if ((n = recvmsg(fd, &msg, 0)) <= 0)	/* riceve il messaggio dal socket */
    return(n);

  if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
       cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
    if (cmptr->cmsg_level != SOL_SOCKET) {
       fprintf(stderr, "livello di controllo != SOL_SOCKET\n");
       exit(1);
    }		
    if (cmptr->cmsg_type != SCM_RIGHTS) {
       fprintf(stderr, "tipo di controllo != SCM_RIGHTS\n");
       exit(1);
    }		
    *recvfd = *((int *) CMSG_DATA(cmptr));
  } else
    *recvfd = -1;		/* descrittore non passato */

  return(n);
}
