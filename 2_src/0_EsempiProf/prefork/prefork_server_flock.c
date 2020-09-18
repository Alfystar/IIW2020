/* Server TCP con preforking e file locking per accept
   Processo padre
   Ultima revisione: 17 ottobre 2008 */

#include <signal.h>
#include <wait.h>
#include "basic.h"
#include "prefork_flock.h"

typedef void Sigfunc(int);

Sigfunc *signal(int signum, Sigfunc *handler);

static int nchildren;
static pid_t *pids;

int main(int argc, char **argv) {
    int listensd, i;
    struct sockaddr_in servaddr;
    socklen_t addrlen;
    void sig_int(int);

    if (argc != 2) {
        fprintf(stderr, "utilizzo: prefork_flock <#children>\n");
        exit(1);
    }
    nchildren = atoi(argv[1]);

    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("errore in socket");
        exit(1);
    }

    memset((void *) &servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        perror("errore in bind");
        exit(1);
    }

    if (listen(listensd, BACKLOG) < 0) {
        perror("errore in listen");
        exit(1);
    }

    pids = (pid_t *) calloc(nchildren, sizeof(pid_t));
    if (pids == NULL) {
        fprintf(stderr, "errore in calloc");
        exit(1);
    }

    my_lock_init("/tmp/lock.XXXXXX"); /* un file di lock per tutti i processi child */
    for (i = 0; i < nchildren; i++)
        pids[i] = child_make(i, listensd, addrlen);    /* il padre ritorna */

    if (signal(SIGINT, sig_int) == SIG_ERR) {
        fprintf(stderr, "errore in signal");
        exit(1);
    }

    for (;;)
        pause();    /* fanno tutto i processi figli */
}

Sigfunc *signal(int signum, Sigfunc *func) {
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

void sig_int(int signo) {
    int i;
    void pr_cpu_time(void);

    /* termina tutti i processi child */
    for (i = 0; i < nchildren; i++)
        kill(pids[i], SIGTERM);
    while (wait(NULL) > 0);    /* attende per tutti i processi child */

    if (errno != ECHILD) {
        fprintf(stderr, "errore in wait");
        exit(1);
    }

    pr_cpu_time();
    exit(0);
}
