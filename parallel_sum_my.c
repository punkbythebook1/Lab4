#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "utils.h"

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs*);

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */
    uint32_t threads_num = -1;
  uint32_t array_size = -1;
  uint32_t seed = -1;
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
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
            if(array_size <= 0 ) return -1;
            break;
          case 2:
            threads_num = atoi(optarg);
            // your code here
            // error handling
            if(threads_num <= 0 ) return -1;
            break;
        
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }
  
  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n",
           argv[0]);
    return 1;
  }
  

  pthread_t *threads = malloc(sizeof(pthread_t) * threads_num);

  /*
   * TODO:
   * your code here
   * Generate array here
   */

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct SumArgs *args = malloc(sizeof(struct SumArgs) * threads_num);
  
  for(uint32_t i = 0; i < threads_num; i++){
      args[i].array = array;
      args[i].begin = i*(int)(array_size / threads_num);
      if(i < (threads_num - 1) )
        args[i].end = i*(int)(array_size / threads_num)+(int)(array_size / threads_num) - 1;
      else
        args[i].end = array_size - 1; 
  }
  
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }
  
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    for(int i = 0; i < array_size;i++)printf("%d ",array[i]);
    printf("\n");
      printf("Elapsed time: %fms\n", elapsed_time);

    free(array);
     printf("Total: %d\n", total_sum);
}