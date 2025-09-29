#include <adf.h>

void kernel_1(adf::input_buffer<int32> & bufin,  adf::output_buffer<int32> & bufout) {
  int32 i;
  int32* inItr = bufin.data();
  int32* outItr = bufout.data();
  for (i=0;i<16;i++) {
    int j = *inItr++;
    *outItr++ = j + 100;
  }; 
}
