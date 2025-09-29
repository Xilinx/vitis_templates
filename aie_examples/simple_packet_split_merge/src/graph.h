

#include <adf.h>
#include "kernels.h"

using sim_platform = adf::simulation::platform<16,8>;

class ExplicitPacketSwitching: public adf::graph { 
private:
  adf:: kernel me_kernel1;
  adf:: kernel me_kernel2;

  adf:: pktsplit<2> sp;
  adf:: pktmerge<2> mg;
public:

  adf::input_plio in;
  adf::output_plio out;
  
  ExplicitPacketSwitching() { 

    in  = adf::input_plio::create(adf::plio_32_bits, "data/input.txt");
    out = adf::output_plio::create(adf::plio_32_bits, "data/output.txt");

    me_kernel1 = adf::kernel::create(me_kernel);
    adf::runtime<ratio>(me_kernel1) = 0.9;
    adf::source(me_kernel1) = "kernels/me_kernel.cc";

    me_kernel2 = adf::kernel::create(me_kernel);
    adf::runtime<ratio>(me_kernel2) = 0.9;
    adf::source(me_kernel2) = "kernels/me_kernel.cc";

    sp = adf::pktsplit<2>::create();
    mg = adf::pktmerge<2>::create();

    adf::connect<adf::pktstream> (in.out[0], sp.in[0]);

    adf::connect<adf::pktstream, adf::stream> (sp.out[0], me_kernel1.in[0]);
    adf::connect<adf::pktstream, adf::stream> (sp.out[1], me_kernel2.in[0]);
    adf::connect<adf::pktstream> (me_kernel1.out[0], mg.in[0]);
    adf::connect<adf::pktstream> (me_kernel2.out[0], mg.in[1]);

    adf::connect< adf::pktstream> (mg.out[0], out.in[0]);

  } 
};
