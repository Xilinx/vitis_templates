
#include <adf.h>
#include "kernels.h"

using namespace adf;

class StreamSwitchFIFO : public adf::graph {
  private:
    kernel dist;
    kernel comp0;
    kernel comp1;
    kernel aggr;
  public:
    input_plio in;
    output_plio out;

    StreamSwitchFIFO() { 
      in  = input_plio::create(plio_32_bits, "data/input.txt");
      out = output_plio::create(plio_32_bits, "data/output.txt");

      dist = kernel::create(distribute);
      comp0= kernel::create(compute0);
      comp1= kernel::create(compute1);
      aggr = kernel::create(aggregate);
    
      connect< stream > n0(in.out[0], dist.in[0]);
      connect< stream > n1(dist.out[0], comp0.in[0]);
      connect< stream > n2(dist.out[0], comp1.in[0]);
      connect< stream > n3(comp0.out[0], aggr.in[0]);
      connect< stream > n4(comp1.out[0], aggr.in[1]);
      connect< stream > n5(aggr.out[0], out.in[0]);

      source(dist) =  "kernels/distribute.cc";
      source(comp0) = "kernels/compute0.cc";
      source(comp1) = "kernels/compute1.cc";
      source(aggr) =  "kernels/aggregate.cc";

      runtime<ratio>(dist) = 0.7;
      runtime<ratio>(comp0) = 0.7;
      runtime<ratio>(comp1) = 0.7;
      runtime<ratio>(aggr) = 0.7;

      // Pin down locations so as to get predictable results.
      location<kernel>(dist) = adf::tile(0,0);
      location<kernel>(comp0) = adf::tile(1,0);
      location<kernel>(comp1) = adf::tile(1,1);
      location<kernel>(aggr) = adf::tile(2,0);

      fifo_depth(n2) = 36; // Removing FIFO will create deadlock 
      fifo_depth(n4) = 36; // Removing FIFO will create deadlock  
  };
  };


