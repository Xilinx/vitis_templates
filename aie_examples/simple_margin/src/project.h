
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
    
    in  = input_plio::create(plio_32_bits,"data/input.txt");
    out = output_plio::create(plio_32_bits,"data/output.txt");

    first = kernel::create(simple_margin<NUM_MARGIN_SAMPLES>);
    second = kernel::create(simple_margin<0>);
    connect(in.out[0], first.in[0]);
    connect(first.out[0], second.in[0]);
    connect(second.out[0], out.in[0]);
    dimensions(first.in[0]) = { NUM_SAMPLES };
    dimensions(first.out[0]) = { NUM_SAMPLES };
    dimensions(second.in[0]) = { NUM_SAMPLES };
    dimensions(second.out[0]) = { NUM_SAMPLES };

    source(first) = "kernels/kernels.cc";
    source(second) = "kernels/kernels.cc";

    runtime<ratio>(first) = 0.1;
    runtime<ratio>(second) = 0.1;

  }
};
