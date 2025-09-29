#include <stdlib.h>
#include <stdio.h>
#include <adf.h>
#include <kernels.h>
//#include "window.h"
//#include <adf/window/window.h>

static int odd = 0;

void super_kernel(adf::input_async_buffer<int32> & data, adf::input_buffer<int32> & weight, adf::output_async_buffer<int32> & result) {
  int32 i;
  int32* weightItr = weight.data();
  int32* resultItr = result.data();
  printf("super_kernel starting...\n");
  printf("odd = %d\n", odd);
  data.acquire();
  int32* dataItr = data.data();
  printf("Acquired lock for data.\n");
  if (!odd) {  // i.e., even iteration
    result.acquire();
    resultItr = result.data();
    printf("Acquired lock for result.\n");
  }
  else
    resultItr+=8;
  
  for (i=0;i<8;i++) {
    //int32 tmp_data = window_readincr(data);
    int32 tmp_data = *dataItr++;
    printf("super_kernel: data = %d\n", tmp_data);
    int32 tmp_weight = *weightItr++;;
    printf("super_kernel: weight = %d\n", tmp_weight);
    int32 tmp_result = tmp_data * tmp_weight;
    printf("super_kernel: result = %d\n", tmp_result);
    //window_writeincr(result, tmp_result);
    printf("super_kernel: prev result stored = %d\n", *resultItr);
    *resultItr++ = tmp_result;
  }
  data.release();
  printf("Released lock for data.\n");
  if (odd) {  // i.e., odd iteration
    result.release();
    printf("Released lock for result.\n");
  }
  odd = 1 - odd;
  printf("super_kernel done.\n");
};