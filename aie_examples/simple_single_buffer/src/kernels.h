
#ifndef FUNCTION_KERNELS_H
#define FUNCTION_KERNELS_H

  template<int M>
  void simple_margin(adf::input_buffer<cint16, adf::extents<adf::inherited_extent>, adf::margin<M>> & in, adf::output_buffer<cint16> & out);

#endif
