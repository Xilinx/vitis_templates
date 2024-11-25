
#ifndef KERNEL_H

#include <adf/window/types.h>
#include <adf/stream/types.h>

extern "C" { 
  void super_kernel(adf::input_async_buffer<int32> & data, adf::input_buffer<int32> & weight, adf::output_async_buffer<int32> & result);
};


#endif
