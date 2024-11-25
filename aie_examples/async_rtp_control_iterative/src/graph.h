
#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <adf.h>
#include "kernels.h"

using namespace adf;

class rtpGraph : public adf::graph {
private:
    kernel k1;

public:
    input_plio streamInput;
    input_port rtpInput;
    output_plio streamOutput;
    inout_port rtpOutput;

    rtpGraph()
    {
        streamInput = input_plio::create(plio_32_bits,"data/input.txt");
        streamOutput = output_plio::create(plio_32_bits,"data/output.txt");

        k1 =  kernel::create(multiply);
        source(k1) = "src/multiply.cpp";
        runtime<ratio>(k1) = 0.1;

        connect<stream>(streamInput.out[0], k1.in[0]);
        connect<parameter>(rtpInput, async(k1.in[1]));
        connect<stream>(k1.out[0], streamOutput.in[0]);
        connect<parameter>(k1.inout[0], rtpOutput); //default async for unconnected inout RTP
    }
};

#endif
