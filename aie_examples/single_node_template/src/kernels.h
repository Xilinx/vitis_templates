#include <adf.h>

#ifndef FUNCTION_KERNELS_H
#define FUNCTION_KERNELS_H
#define NUM_SAMPLES 8
void vitis_single_node_templates(adf::input_buffer<int32> & bufin, adf::output_buffer<int32> & bufout, input_stream_int32* sin, output_stream_int32* sout, const int (&vectorIn)[NUM_SAMPLES], int factorIn, int (&vectorOut)[NUM_SAMPLES], int& factorOut );

extern int32 LUT[NUM_SAMPLES];

#endif
