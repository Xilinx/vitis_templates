#include <adf.h>

void kernel1(adf::input_buffer<int32> & bufin0, adf::input_buffer<int32> & bufin1, adf::output_buffer<int32> & bufout) {
  int32 i;
  int32* in0Itr = bufin0.data();
  int32* in1Itr = bufin1.data();
  int32* outItr = bufout.data();
  for (i=0;i<16;i++) {
    int j = *in0Itr++;
    int k = *in1Itr++;
    *outItr++ = j + k + 100;
  }; 
}
