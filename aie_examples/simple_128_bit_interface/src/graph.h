#include <adf.h>
#include <kernels.h>

using namespace adf;

class MEPL128BitClass : public adf::graph 
{
private:
  adf::kernel first;
public:
  adf::input_plio in;
  adf::output_plio out;
  MEPL128BitClass() 
  { 
    in = input_plio::create("TestLogicalNameIn",plio_128_bits,"data/input.txt");
    out = output_plio::create("TestLogicalNameOut",plio_128_bits,"data/output.txt");

    first = adf::kernel::create(simple);
    adf::connect< adf::stream > net2(in.out[0], first.in[0]);
    adf::connect< adf::stream > net1(first.out[0], out.in[0]);
    adf::source(first) = "kernels/kernels.cc";
    adf::runtime<ratio>(first) = 0.5;
  } 
};
