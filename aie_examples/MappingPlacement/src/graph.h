
#include <adf.h>
#include "kernels.h"

#define NUMCORES 2 

template <int COLS, int ROWS, int STARTCOL, int STARTROW>
class indep_nodes_graph : public adf::graph {

 private:
 
  adf::kernel kr[NUMCORES];
  
 public:
   adf::input_plio datain[NUMCORES] ;
   adf::output_plio dataout[NUMCORES] ;
  
 indep_nodes_graph() {
 
  for (int i=0; i<NUMCORES; i++) {
    datain[i]  = adf::input_plio::create(adf::plio_32_bits, "data/input_core" + std::to_string(i) + ".txt");
    dataout[i] = adf::output_plio::create(adf::plio_32_bits, "data/output_core" + std::to_string(i) + ".txt");
  }

  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      int k = i*ROWS + j;
      kr[k] = adf::kernel::create(kernel_1);
      adf::source(kr[k])  = "kernels/kernel.cc";
      adf::runtime<ratio>(kr[k]) = 0.9;
      adf::location<adf::kernel>(kr[k]) = adf::tile(STARTCOL+i, STARTROW+j); // kernel location
      adf::location<adf::buffer>(kr[k].in[0]) = { adf::address(STARTCOL+i, STARTROW+j, 0x0), adf::address(STARTCOL+i, STARTROW+j, 0x2000) }; // buffer location
      adf::location<adf::kernel>(kr[k]) = adf::location<adf::buffer>(kr[k].out[0]); // relative kernel buffer location
      adf::location<adf::stack>(kr[k]) = adf::bank(STARTCOL+i, STARTROW+j, 2); // stack location
    }
  }

  for (int i=0; i<NUMCORES; i++) {
    adf::connect(datain[i].out[0], kr[i].in[0]);
    adf::connect(kr[i].out[0], dataout[i].in[0]);
    adf::dimensions(kr[i].in[0]) = { 16 };
    adf::dimensions(kr[i].out[0]) = { 16 };
  }

  
 };
};
