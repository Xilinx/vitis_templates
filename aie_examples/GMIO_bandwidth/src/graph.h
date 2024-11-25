
#include "mysquare.h"
#include <adf.h>

using namespace adf;

struct myGraph : public adf::graph {

public :
adf::kernel sq;


  adf::input_gmio in;
  adf::output_gmio  out;

  myGraph() {

    in  = input_gmio::create("gm1", 64, 1);
    out = output_gmio::create("gm2", 64, 1);

    sq = adf::kernel::create( mysquare ) ;
    adf::source(sq) = "src/kernels/mysquare.cc";

    runtime<ratio>(sq) = 0.6;

    adf::connect(in.out[0], sq.in[0]);
    adf::connect(sq.out[0],out.in[0]);
    dimensions(sq.in[0]) = { 32 };
    dimensions(sq.out[0]) = { 32 };

    adf::location<kernel>(sq) = adf::tile(1,1);
  }
};

