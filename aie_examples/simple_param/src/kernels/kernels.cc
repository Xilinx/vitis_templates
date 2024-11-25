/* A simple kernel with a scalar input parameter
 */
#include <adf.h>
#include "include.h"

void simple_param(adf::input_buffer<cint16> & in, adf::output_buffer<cint16> & out, int select) {
  cint16 c1, c2;
  cint16* inItr = in.data();
  cint16* outItr = out.data();
  for (unsigned i=0; i<NUM_SAMPLES/4; i++) {
    c1 = *inItr++;
    c2.real = c1.real+c1.imag-select;
    c2.imag = c1.real-c1.imag+select;
    *outItr++ = c2;
  }
}
