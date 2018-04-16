#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
//ps aux | grep -w Z search zombies
int main ()
{
  pid_t child_pid;

  child_pid = fork ();
  if (child_pid >= 0) {
     if(child_pid == 0){
            // signal(SIGCHLD, SIG_IGN);
            // signal(SIGHUP, SIG_IGN);
         exit(-1);
     }
     sleep(5);   
  }
  else {
    exit(0);
  }
  exit(0);
}