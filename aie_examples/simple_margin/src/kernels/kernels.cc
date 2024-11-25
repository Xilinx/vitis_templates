/* A simple kernel
 */
#include <adf.h>
#include "include.h"

template<int M>
void simple_margin(adf::input_buffer<cint16, adf::extents<adf::inherited_extent>, adf::margin<M>> & in, adf::output_buffer<cint16> & out) {
  cint16 c1, c2;
  cint16* inItr = in.data();
  cint16* outItr = out.data();
  for (unsigned i=0; i<NUM_SAMPLES; i++) {
    c1 = *inItr++;
    c2.real = c1.real+c1.imag;
    c2.imag = c1.real-c1.imag;
    *outItr++ = c2;
  }
}
