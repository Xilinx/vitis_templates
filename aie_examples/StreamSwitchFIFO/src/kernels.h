
#ifndef KERNEL_H

#include <adf/window/types.h>
#include <adf/stream/types.h>


  void distribute(input_stream_int32 * instream, output_stream_int32 * outstream);
  void compute0(input_stream_int32 * instream, output_stream_int32 * outstream);
  void compute1(input_stream_int32 * instream, output_stream_int32 * outstream);
  void aggregate(input_stream_int32 * instream0, input_stream_int32 * instream1, output_stream_int32 * outstream);



#endif
