#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  
  // your code here
    for(int i = 0;i < end - begin +1 ;i++){
      if( array[begin + i] > min_max.max)
      min_max.max = array[begin + i];
      if(array[begin + i] < min_max.min)  
        min_max.min = array[begin + i];
    }
 return min_max;
}
