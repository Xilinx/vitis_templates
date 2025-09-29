#include <adf.h>
#include "include.h"

void consumer(const int32 (&d)[NUM_SAMPLES], output_stream_int32* out) {
  int i;
  for (i=0; i<NUM_SAMPLES; i++) 
    writeincr(out, d[i]);
}
