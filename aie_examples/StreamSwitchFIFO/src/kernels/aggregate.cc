#include <adf.h>

void aggregate(input_stream_int32 * instream0, input_stream_int32 * instream1, output_stream_int32 * outstream) {
  int32 i;
  for (i=0;i<96;i++) {
    int32 j0 = readincr(instream0);
    int32 j1 = readincr(instream1);
    writeincr(outstream, j0+j1);
  }
};
