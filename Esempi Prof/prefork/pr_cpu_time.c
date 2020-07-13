#include <sys/resource.h>
#include <sys/time.h>
#include	"basic.h"

void pr_cpu_time(void)
{
  double	user, sys;
  struct rusage	myusage, childusage;

  if (getrusage(RUSAGE_SELF, &myusage) < 0) {
    fprintf(stderr, "errore in getrusage");
    exit(1);
  }
  
  if (getrusage(RUSAGE_CHILDREN, &childusage) < 0) {
    fprintf(stderr, "errore in getrusage");
    exit(1);
  }
	
  user = (double) myusage.ru_utime.tv_sec +
		myusage.ru_utime.tv_usec/1000000.0;
  user += (double) childusage.ru_utime.tv_sec +
		 childusage.ru_utime.tv_usec/1000000.0;
  sys = (double) myusage.ru_stime.tv_sec +
		 myusage.ru_stime.tv_usec/1000000.0;
  sys += (double) childusage.ru_stime.tv_sec +
		 childusage.ru_stime.tv_usec/1000000.0;

  printf("\nuser time = %g, sys time = %g\n", user, sys);
}
