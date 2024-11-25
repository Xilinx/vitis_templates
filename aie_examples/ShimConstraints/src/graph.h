
#include <adf.h>
#include "kernels.h"

class indep_nodes_graph : public adf::graph {
 private:
  adf::kernel k1;
 public:
   adf::input_plio datain0 ;
   adf::input_plio datain1 ;
   adf::output_plio dataout;
  
 indep_nodes_graph() {
    datain0 = adf::input_plio::create("Datain0",adf::plio_32_bits,"data/input_A_core0.txt");
    datain1 = adf::input_plio::create("Datain1",adf::plio_32_bits,"data/input_B_core0.txt");
    dataout = adf::output_plio::create("Dataout0",adf::plio_32_bits,"data/output.txt");

    k1 = adf::kernel::create(kernel1);
    adf::source(k1)  = "kernels/kernel1.cc";
    adf::runtime<ratio>(k1) = 0.9;
    adf::connect(datain0.out[0], k1.in[0]);
    adf::connect(datain1.out[0], k1.in[1]);
    adf::connect(k1.out[0], dataout.in[0]);
    adf::dimensions(k1.in[0]) = { 16 };
    adf::dimensions(k1.in[1]) = { 16 };
    adf::dimensions(k1.out[0]) = { 16 };
 };

};
