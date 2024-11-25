#include <adf.h>

void compute1(input_stream_int32 * instream, output_stream_int32 * outstream) {
  int32 i;
  for (i=0;i<96;i++) {
    int32 j = readincr(instream);
    writeincr(outstream, j);
  }
};
