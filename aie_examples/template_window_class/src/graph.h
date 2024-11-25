#pragma once

#include "adf.h"
#include "fir.h"

using namespace adf;

class mygraph : public graph
{
public:
    input_plio in1, in2;
    output_plio out1, out2;
    kernel k1, k2;

    mygraph()
    {
        in1   = input_plio::create(plio_32_bits, "data/noise1.txt"); 
        in2   = input_plio::create(plio_32_bits, "data/noise2.txt"); 
        out1  = output_plio::create(plio_32_bits, "data/filtered1.txt"); 
        out2  = output_plio::create(plio_32_bits, "data/filtered2.txt"); 

        //see lab8.3 for narrow filter coefficients
        k1 = kernel::create_object<FIR<12, int32>>(std::vector<int>({ 180, 89, -80, -391, -720, -834, -478, 505, 2063, 3896, 5535, 6504 }), 8);
        runtime<ratio>(k1) = 0.1;
        source(k1) = "src/fir.cpp";

        //see lab8.3 for wide filter coefficients
        k2 = kernel::create_object<FIR<15, int32>>(std::vector<int>({ -21, -249, 319, -78, -511, 977, -610, -844, 2574, -2754, -1066, 18539, 0, 0, 0 }), 8);
        runtime<ratio>(k2) = 0.1;
        source(k2) = "src/fir.cpp";

        connect(in1.out[0], k1.in[0]);
        connect(in2.out[0], k2.in[0]);

        connect(k1.out[0], out1.in[0]);
        connect(k2.out[0], out2.in[0]);
        dimensions(k1.in[0]) = { 8 };
        dimensions(k1.out[0]) = { 8 };
        dimensions(k2.in[0]) = { 8 };
        dimensions(k2.out[0]) = { 8 };
    }
};
