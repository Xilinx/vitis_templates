
#include <adf.h>
#include "kernels.h"
#include "kernels/include.h"

using namespace adf;

class chainedGraph : public adf::graph {
public:
  kernel first;
  kernel second;
  output_plio out;

  chainedGraph() {
    out = output_plio::create(plio_32_bits, "data/output.txt");

    first =  kernel::create(producer);
    source(first) = "kernels/producer.cc";
    runtime<ratio>(first) = 0.9;
    second = kernel::create(consumer);
    source(second) = "kernels/consumer.cc";
    runtime<ratio>(second) = 0.9;

    connect<stream>(second.out[0], out.in[0]);
    // connect<parameter>(first.inout[0], second.in[0]);
    sync(first.inout[0]);
    sync(second.in[0]);

  }
};

