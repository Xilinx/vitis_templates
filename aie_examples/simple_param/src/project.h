
#include <adf.h>
#include "kernels.h"
#include "src/kernels/include.h"

using namespace adf;

class parameterGraph : public adf::graph {
private:
  kernel first;

public:
  input_port select_value;
  input_plio in;
  output_plio out;
  parameterGraph() {

    in = input_plio::create(plio_32_bits,"data/input.txt");
    out = output_plio::create(plio_32_bits,"data/output.txt");
    
    first = kernel::create(simple_param);

    connect(in.out[0], first.in[0]);
    connect(first.out[0], out.in[0]);
    connect<parameter>(select_value, first.in[1]);

    dimensions(first.in[0]) = { NUM_SAMPLES/4 };
    dimensions(first.out[0]) = { NUM_SAMPLES/4 };

    source(first) = "kernels/kernels.cc";
    runtime<ratio>(first) = 0.1;
  }
};
