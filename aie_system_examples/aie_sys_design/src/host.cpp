

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fstream>
#include "input.h"
#include "golden.h"
#include <cstring>
#include <iostream>

#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_hw_context.h"
#include "experimental/xrt_graph.h"

#include "adf/adf_api/XRTConfig.h"

#define INPUT_SIZE  128
#define OUTPUT_SIZE 256

#define NO_OF_ITERATIONS  4


int main(int argc, char ** argv)
{

	//////////////////////////////////////////
	// Open xclbin
	//////////////////////////////////////////

    if(argc <2) {
		std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
		return EXIT_FAILURE;
	}

    char* xclbinFilename = argv[1];


    auto device = xrt::device(0);
    auto uuid = device.load_xclbin(xclbinFilename);
    xrt::hw_context hw_ctx(device, uuid);


//This dictates number of iterations to run through.
    long itr = NO_OF_ITERATIONS;

//calculate input/output data size in number of samples.
    int sizeIn = INPUT_SIZE * itr;
    int sizeOut = OUTPUT_SIZE * itr;

    size_t input_size_in_bytes = sizeIn * sizeof(int);
    size_t output_size_in_bytes = sizeOut * sizeof(int);

    //Manage/map input/output file

	//////////////////////////////////////////
	// input memory
	// No cache no sync seems not working. Should ask SSW team to investigate.
	//
    ////////////////////////////////////////

    xrt::bo in_bohdl(hw_ctx, input_size_in_bytes, 0);
    auto in_bomapped = in_bohdl.map<short int*>();
    memcpy(in_bomapped, cint16input, input_size_in_bytes);
    printf("Input memory virtual addr 0x%llx\n", in_bomapped);




	//////////////////////////////////////////
	// output memory
	//////////////////////////////////////////

	xrt::bo out_bohdl(hw_ctx, output_size_in_bytes, 0);
    auto out_bomapped = out_bohdl.map<int*>();
	printf("Output memory virtual addr 0x%llx\n", out_bomapped);


	//////////////////////////////////////////
	// mm2s ip
	//////////////////////////////////////////

	xrt::kernel mm2s_khdl(hw_ctx, "mm2s");
	xrt::run mm2s_rhdl(mm2s_khdl);
    mm2s_rhdl.set_arg(0, in_bohdl);
    mm2s_rhdl.set_arg(2, sizeIn);
    mm2s_rhdl.start();
	printf("run mm2s\n");

	//////////////////////////////////////////
	// s2mm ip
	//////////////////////////////////////////

	xrt::kernel s2mm_khdl(hw_ctx, "s2mm");
	xrt::run s2mm_rhdl(s2mm_khdl);
    s2mm_rhdl.set_arg(0, out_bohdl);
    s2mm_rhdl.set_arg(2, sizeOut);
    s2mm_rhdl.start();
	printf("run s2mm\n");

    //////////////////////////////////////////
    // polar clip ip
    //////////////////////////////////////////

    xrt::kernel polar_clip_khdl(hw_ctx, "polar_clip");
    xrt::run polar_clip_rhdl(polar_clip_khdl);
    polar_clip_rhdl.set_arg(2, sizeOut);
    polar_clip_rhdl.start();
    printf("run polar_clip\n");




    printf("xrtGraphOpen\n");
    xrt::graph ghdl(device, uuid, "clipgraph");
    printf("xrtGraphRun\n");
    ghdl.run(itr);

    //////////////////////////////////////////
	// wait for mm2s done
	//////////////////////////////////////////
    auto state = mm2s_rhdl.wait();
    std::cout << "mm2s completed with status(" << state << ")\n";

    //////////////////////////////////////////
    // wait for polar clip done
    //////////////////////////////////////////
    state = polar_clip_rhdl.wait();
    std::cout << "polar_clip completed with status(" << state << ")\n";

    //////////////////////////////////////////
    // wait for s2mm done
	//////////////////////////////////////////

	state = s2mm_rhdl.wait();
    std::cout << "s2mm completed with status(" << state << ")\n";


    ghdl.end(0);
    printf("xrtGraphEnd..\n");


	//////////////////////////////////////////
	// compare results
	//////////////////////////////////////////

    int errCnt = 0;
    for (int i = 0; i < sizeOut; i++) {
        if (out_bomapped[i] != golden[i]) {
            printf("ERROR: Test failed! Error found in sample %d: golden: %d, hardware: %d\n", i, golden[i], out_bomapped[i]);
            errCnt++;
        }
    }


    //////////////////////////////////////////
	// clean up XRT
	//////////////////////////////////////////

    std::cout << "Releasing remaining XRT objects...\n";

    std::cout << "TEST " << (errCnt ? "FAILED" : "PASSED") << std::endl;
    return (errCnt ? EXIT_FAILURE :  EXIT_SUCCESS);
}
