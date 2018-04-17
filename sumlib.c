#include <stdio.h>
static struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args) {
  int sum = 0;
  // TODO: your code here
  int begin = args->begin;
  int end = args->end;
  int *array = args->array;
  
  for(int i = 0;i < (end - begin + 1);i++)
    sum+=array[begin+i];
  
  return sum;
}