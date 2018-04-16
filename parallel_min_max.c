#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"
/*
signal(SIGALRM, signal_callback_handler);
  unsigned int timeout = 1;
  alarm(timeout);
*/

pid_t* child_pids = NULL;
int pnum = -1;

void signal_callback_handler(int signum){
   if(signum == SIGALRM)
      for(int i =0 ; i< pnum ;i++)
            kill(child_pids[i],SIGKILL); 
      printf("KILL_SIGNAL_SENDS\n");
   // exit(-1);
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  bool with_files = false;
  int timeout = 0;
    
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options , &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if(seed <= 0 ) return -1;
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if(seed <= 0 ) return -1;
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            if(seed <= 0 ) return -1;
            break;
          case 3:
            with_files = true;
            break;
         case 4:
            timeout = atoi(optarg);
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }
  
  ////// creates pipes for eache forked process
   FILE **file_discr = NULL;
   int **pipe_discr = NULL;
   
  if(with_files){
     file_discr = malloc(sizeof(FILE*) * pnum);  
    
     for(int i = 0; i < pnum;i++){
       char str[256];
       sprintf(str,"/tmp/%d",i);
       file_discr[i] = fopen(str,"w+"); 
   }

  }else{
      
      pipe_discr = malloc(sizeof(int*) * pnum);
      for(int i = 0; i < pnum;i++){
       pipe_discr[i] = malloc(sizeof(int) * 2);
       if(pipe(pipe_discr[i]) == -1 )
            return -1;
      }
  }
 ///////
  
 
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  child_pids = malloc(sizeof(pid_t) * pnum);
  
 
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
 
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    child_pids[i] = child_pid;
    if (child_pid >= 0) {
    
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        sleep(10);
        // parallel somehow

        if (with_files) {
          // use files here
           struct MinMax min_max;
          if(i < (pnum - 1) )
           min_max = GetMinMax(array,i*(int)(array_size / pnum),i*(int)(array_size / pnum)+
          (int)(array_size / pnum) - 1 );
          else
          min_max = GetMinMax(array, i*(int)(array_size / pnum),array_size - 1);
                   printf("max pid %d",min_max.max);       
                printf("\n");
                fprintf(file_discr[i],"%d %d",min_max.min,min_max.max);
                fclose(file_discr[i]);
        } else {
          // use pipe here
          struct MinMax min_max;
          if(i < (pnum - 1) )
           min_max = GetMinMax(array,i*(int)(array_size / pnum),i*(int)(array_size / pnum)+
          (int)(array_size / pnum) - 1 );
          else
          min_max = GetMinMax(array, i*(int)(array_size / pnum),array_size - 1);
         printf("max pid %d",min_max.max);
         printf("\n");
        
         FILE *f = fdopen(pipe_discr[i][1],"w");
         fprintf(f,"%d %d",min_max.min,min_max.max);
         
         fclose(f);
         
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  
  }
  
  if(timeout > 0){
  signal(SIGALRM, signal_callback_handler);
  alarm(timeout);
  }
  int kill_exits = 0;
  while (active_child_processes > 0) {
    // your code here
    int status;
    wait(&status);
    printf("child ret -->%d\n",(int)WIFEXITED(status)); //return true if child procees exit normal
    if((int)WIFEXITED(status) == 0)
        kill_exits++;
    active_child_processes -= 1;
  }  
  printf("no childs \n");
    if(kill_exits == pnum){
        printf("all childs is killed!!\n");
        exit(-1);
    }
        
    
 
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;


  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
      char str[256];
       sprintf(str,"/tmp/%d",i);
      FILE *f = fopen(str,"r+");
      fscanf(f,"%d %d",&min,&max);
      fclose(f);
    } else {
      // read from pipes
     close(pipe_discr[i][1]);
     FILE *f = fdopen(pipe_discr[i][0],"r");
      fscanf(f,"%d %d",&min,&max);
      fclose(f);
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    
  //for(int i = 0; i < array_size;i++)printf("%d ",array[i]);
  //printf("\n");
  free(array);
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  if(with_files)printf("by file\n");
  else printf("by pipe\n");
  fflush(NULL);
  return 0;
}
