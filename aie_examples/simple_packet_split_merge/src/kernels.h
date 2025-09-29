

#ifndef KERNEL_H
#define KERNEL_H

#include <adf.h>

void pl_source_kernel(input_stream_int32 * in, output_pktstream * out) ;
void pl_sink_kernel(input_pktstream * in, output_stream_int32 * out1, output_stream_int32 * out2) ;
void me_kernel(input_stream_int32 * in, output_pktstream * out);

#endif
