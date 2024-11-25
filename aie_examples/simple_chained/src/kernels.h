
#ifndef FUNCTION_KERNELS_H
#define FUNCTION_KERNELS_H
#include "kernels/include.h"

  void producer(int32 (&)[NUM_SAMPLES] );
  void consumer(const int32 (&)[NUM_SAMPLES], output_stream_int32*);

#endif
