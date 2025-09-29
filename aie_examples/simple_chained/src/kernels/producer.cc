#include <adf.h>
#include "include.h"

static int state = 0;

void producer(int32 (&d)[NUM_SAMPLES]) {
  int i;
  for (i=0; i<NUM_SAMPLES; i++) 
    d[i] = i+state;
  state++;
}
