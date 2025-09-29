#include <adf.h>

void compute0(input_stream_int32 * instream, output_stream_int32 * outstream) {
  int32 i;
  int32 tmpin[96], tmpout[96];
  for (i=0;i<96;i++) {
    tmpin[i] = readincr(instream);
  }
  for (i=0;i<96;i++) {
    tmpout[i] = tmpin[i] + 1;
  }
  for (i=0;i<96;i++) {
    writeincr(outstream, tmpout[i]);
  }
};
