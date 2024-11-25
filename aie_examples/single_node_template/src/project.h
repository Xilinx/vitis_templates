#include <adf.h>
#include "kernels.h"

using namespace adf;

class simpleGraph : public adf::graph {
private:
  kernel first;     
  parameter lut0;

public:
  input_plio in0, in1;
  input_port vectorInput, factorInput; 
  inout_port vectorOutput, factorOutput;
  output_plio out0, out1;
  simpleGraph()
  {
    in0 = input_plio::create(plio_32_bits, "data/input_1.txt");
    in1 = input_plio::create(plio_32_bits, "data/input_1.txt");

    out0 = output_plio::create(plio_32_bits, "data/output_1.txt");
    out1 = output_plio::create(plio_32_bits, "data/output_2.txt");
    
    first = kernel::create(vitis_single_node_templates);
    connect(in0.out[0], first.in[0]); 
    dimensions(first.in[0]) = { 8 };
    //single_buffer(first.in[0]);               //uncomment for single buffer, by default double buffer will be used.
    connect(first.out[0], out0.in[0]);
    dimensions(first.out[0]) = { 8 };

    lut0 = parameter::array (LUT);
    connect<> (lut0, first);

    connect<stream> net2 (in1.out[0], first.in[1]);  //net connections for stream-stream 
    connect<stream> net3 (first.out[1], out1.in[0]);
    
    connect<parameter> (vectorInput, first.in[2]);    //connection for RTP_array
    connect<parameter> (sync(first.inout[0]), vectorOutput);
    
    connect<parameter> (factorInput, first.in[3]);
    connect<parameter> (sync(first.inout[1]), factorOutput);
    
    source(first) = "src/kernels/vitis_single_node_templates.cc";
    
    runtime<ratio>(first) = 0.1;
    }
};

