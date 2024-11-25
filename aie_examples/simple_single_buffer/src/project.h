
#include <adf.h>
#include "kernels.h"
#include "src/kernels/include.h"

using namespace adf;

class simpleGraph : public adf::graph {
private:
  kernel first;
  kernel second;
public:
  input_plio in;
  output_plio out;
  simpleGraph(){

    in = input_plio::create(plio_32_bits,"data/input.txt");
    out = output_plio::create(plio_32_bits,"data/output.txt");
    first = kernel::create(simple_margin<0>);
    second = kernel::create(simple_margin<NUM_MARGIN_SAMPLES>);
    connect(in.out[0], first.in[0]); // single buffer DMA input
    single_buffer(first.in[0]);
    
    connect(first.out[0], second.in[0]); // single buffer with margin copy
    single_buffer(second.in[0]);
    
    connect(second.out[0], out.in[0]); // double buffer DMA output

    dimensions(first.in[0]) = { NUM_SAMPLES };
    dimensions(first.out[0]) = { NUM_SAMPLES };
    dimensions(second.in[0]) = { NUM_SAMPLES };
    dimensions(second.out[0]) = { NUM_SAMPLES };

    source(first) = "kernels/kernels.cc";
    source(second) = "kernels/kernels.cc";

    adf::location<adf::kernel>(first) = tile(0, 0);
    adf::location<adf::kernel>(second) = tile(0, 1);

    //buffer placement for single buffer ports
    adf::location<buffer>(first.in[0]) = bank(0,1,2);
    adf::location<buffer>(second.in[0]) = address(0,0,0x0000);

    runtime<ratio>(first) = 0.1;
    runtime<ratio>(second) = 0.1;

  }
};

