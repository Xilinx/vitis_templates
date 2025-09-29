#include <stdio.h>
#include <adf.h>

void mysquare(adf::input_buffer<int32> & win, 
              adf::output_buffer<int32> & out) { 

  int32* inItr = win.data();
  int32* outItr = out.data();
  for (unsigned i = 0 ; i < 32 ; i++) {
    int j = *inItr++;
    *outItr++ = j*j;
  }

}
