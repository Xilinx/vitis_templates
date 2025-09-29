#include <adf.h>
void simple(input_stream_int32 *in, output_stream_int32 *out) 
{
  for (int32 j = 0; j < 64; j++)
  {
    int32 val = readincr(in);
    writeincr(out, val);
  }
}
