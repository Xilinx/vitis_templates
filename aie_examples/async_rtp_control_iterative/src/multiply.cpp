
#include "kernels.h"

void multiply(input_stream_int32* in, int factor, output_stream_int32* out, int& rtpOut)
{
    rtpOut = readincr(in) * factor;
    writeincr(out, rtpOut);
}

