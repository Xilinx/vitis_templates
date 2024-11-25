#ifndef __GRAPH_H__
#define __GRAPH_H__
#include <adf.h>
#include "simple_me_kernel.h"

class MEPL64BitInterface : public adf::graph 
{
private:
  adf::kernel me_first;
public:
  adf::input_plio plioIn0;
  adf::output_plio plioOut0;

  MEPL64BitInterface() 
  { 

      plioIn0 = adf::input_plio::create("TestLogicalNameIn",adf::plio_64_bits,"data/input.txt");
      plioOut0 = adf::output_plio::create("TestLogicalNameOut",adf::plio_64_bits,"data/output.txt");

      me_first = adf::kernel::create(simple_me_kernel); 
      adf::source(me_first) = "src/me_kernels/simple_me_kernel.cc";

      adf::connect<adf::stream> net2 (plioIn0.out[0], me_first.in[0]);
      adf::connect<adf::stream> net1 (me_first.out[0], plioOut0.in[0]);

      adf::runtime<ratio>(me_first) = 0.6;
  }
};
#endif
