#include "../kernels.h"

int32 LUT[NUM_SAMPLES] = {1,2,3,4,5,6,7,8};

void vitis_single_node_templates(adf::input_buffer<int32> & bufin, adf::output_buffer<int32> & bufout, input_stream_int32* sin, output_stream_int32* sout, const int (&vectorIn)[NUM_SAMPLES], int factorIn, int (&vectorOut)[NUM_SAMPLES], int& factorOut ){
  int32 bc1, sc1;
  int32* inItr = bufin.data();
  int32* outItr = bufout.data();
  for(int i = 0; i < NUM_SAMPLES; i++)
  {
    
    bc1 = *inItr++;
    bc1 = bc1 + 5 + LUT[i];
    *outItr++ = bc1;
    
    sc1 = readincr(sin);
    sc1 = sc1 + 5;
    writeincr(sout, sc1);

    vectorOut[i] = vectorIn[i] + 5;
    factorOut = factorIn + 5;
	
  }
}
