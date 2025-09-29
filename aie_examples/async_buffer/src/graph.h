
#include <adf.h>
#include "kernels.h"

using namespace adf;

class mygraph : public adf::graph {

  private:
    adf::kernel superkernel;
  public:
//    adf::port<input> datain[2];
//    adf::port<output> dataout[1];
    input_plio  datain[2];
    output_plio dataout;
    mygraph() { 

      // Nodes
      datain[0] = input_plio::create(plio_32_bits, "data/data0.txt");
      datain[1] = input_plio::create(plio_32_bits, "data/weights0.txt");
      dataout   = output_plio::create(plio_32_bits, "data/result0.txt");

      // Create the superkernel
      superkernel = adf::kernel::create(super_kernel);
      adf::source(superkernel) = "src/kernels/super_kernel.cc";
      adf::runtime<ratio>(superkernel) = 0.9;
      adf::location<adf::kernel>(superkernel) = adf::tile(0,0);

      // Edges

      adf::connect(datain[0].out[0], superkernel.in[0]);
      adf::location<adf::buffer>(superkernel.in[0]) = { adf::address(0, 0, 0x4000), adf::address(0, 0, 0x6000) }; // data location constraint
      adf::dimensions(superkernel.in[0]) = { 8 };

      adf::connect(datain[1].out[0], superkernel.in[1]);
      adf::location<adf::buffer>(superkernel.in[1]) = { adf::address(0, 0, 0x5000), adf::address(0, 0, 0x7000) }; // weights location constraint
      adf::dimensions(superkernel.in[1]) = { 8 };

      adf::connect(superkernel.out[0], dataout.in[0]);
      adf::location<adf::buffer>(superkernel.out[0]) = { adf::address(0, 0, 0x0), adf::address(0, 0, 0x2000) }; // output location constraint
      adf::dimensions(superkernel.out[0]) = { 16 };

    };


  };

