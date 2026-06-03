/*
Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: X11
*/


#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

#include "xrt/xrt_device.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_hw_context.h"
#include "xrt/xrt_graph.h"
#include "xrt/experimental/xrt_xclbin.h"

template<typename T>
int load_data_file(std::string fname,T * data,int L)
{
    int d;
    int l=0;

    std::ifstream fin(fname);
    if(!fin) return(-1);

    while(fin>>d)
    {
        data[l++] = (T)d;
        if(l==L) break;
    }
    fin.close();
    return(l);
}


void MatrixMultiply(int8_t* A,int8_t * B, int * C, int M, int K, int N)
{
    for(int i=0;i<M;i++)
    for(int j=0;j<N;j++)
    {
        int s = 0;
        for(int k=0;k<K;k++)
        {
            s += (int)A[i*N+k]*(int)B[j+k*N];
        }
        C[i*N+j] = s;
    }
}

std::string Messages[] = 
{
    "Null",
    "ERT_CMD_STATE_NEW",
    "ERT_CMD_STATE_QUEUED",
    "ERT_CMD_STATE_RUNNING",
    "ERT_CMD_STATE_COMPLETED",
    "ERT_CMD_STATE_ERROR",
    "ERT_CMD_STATE_ABORT",
    "ERT_CMD_STATE_SUBMITTED",
    "ERT_CMD_STATE_TIMEOUT",
    "ERT_CMD_STATE_NORESPONSE",
    "ERT_CMD_STATE_SKERROR",
    "ERT_CMD_STATE_SKCRASHED"
};



int kernel_run_status(std::string Kernel,xrt::run &rhdl)
{
    auto status = (int)rhdl.state();

    std::cout << "Status of " << Kernel << " --> " << Messages[(int)status] << std::endl;
}

int wait_status(std::string Kernel,xrt::run &rhdl)
{
    int status1,status;

    status1 = -1;
    do {
        status = (int)rhdl.state();
        if(status!=status1) 
            std::cout << "Status of " << Kernel << " --> " << Messages[(int)status] << std::endl;
        status1 = status;
    }while (status<3); // NEW or QUEUED
    std::cout << std::endl;
    return(status);
}
#define INPUT_SIZEA (NIterations*sizeM*sizeK)
#define INPUT_SIZEB (NIterations*sizeK*sizeN)
#define OUTPUT_SIZEC (NIterations*sizeM*sizeN)
#define DATAIN_NBYTES_8 1
#define DATAOUT_NBYTES_16 2
#define DATAOUT_NBYTES_32 4
#define NITER NIterations

#define PLIO_NBYTES (PLIOW/8)

int run(int argc, char* argv[]){

    int pl_bramA_size_in_bytes = INPUT_SIZEA*DATAIN_NBYTES_8;
    int pl_bramB_size_in_bytes = INPUT_SIZEB*DATAIN_NBYTES_8;
    int pl_bramC1_size_in_bytes = OUTPUT_SIZEC*DATAOUT_NBYTES_32;
    int pl_bramC2_size_in_bytes = OUTPUT_SIZEC*DATAOUT_NBYTES_16;

    //TARGET_DEVICE macro needs to be passed from gcc command line
    if(argc != 2 ) {
        std::cout << "Usage: " << argv[0] <<" <xclbin> " << std::endl;
        return EXIT_FAILURE;
    }
    char* xclbinFilename = argv[1];

   
    std::cout << "The design will run " << NITER << " Iterations" << std::endl << std::endl;

    std::cout << "Data sizes" << std::endl;
    std::cout << "INPUT_SIZEA: " << INPUT_SIZEA << " --> " << pl_bramA_size_in_bytes << std::endl;
    std::cout << "INPUT_SIZEB: " << INPUT_SIZEB << " --> " << pl_bramB_size_in_bytes <<  std::endl;
    std::cout << "OUTPUT_SIZEC1: " << OUTPUT_SIZEC << " --> " << pl_bramC1_size_in_bytes <<  std::endl;
    std::cout << "OUTPUT_SIZEC2: " << OUTPUT_SIZEC << " --> " << pl_bramC2_size_in_bytes <<  std::endl;



	//////////////////////////////////////////
    // Open xclbin
	//////////////////////////////////////////

    auto device = xrt::device(0); //device index=0
    xrt::xclbin xclbin_obj{std::string{xclbinFilename}};
    auto xclbin_uuid = device.register_xclbin(xclbin_obj);
    xrt::hw_context hw_ctx(device, xclbin_uuid);


	////////////////////////////////////////////////////////
	// Create kernel handles first so group_id() can be used for BO allocation
	///////////////////////////////////////////////////////

	auto mm2sA1_khdl = xrt::kernel(hw_ctx, "mm2s_8_128:{mm2s_8_128_1}");
	auto mm2sB1_khdl = xrt::kernel(hw_ctx, "mm2s_8_128:{mm2s_8_128_2}");
	auto mm2sA2_khdl = xrt::kernel(hw_ctx, "mm2s_8_128:{mm2s_8_128_3}");
	auto mm2sB2_khdl = xrt::kernel(hw_ctx, "mm2s_8_128:{mm2s_8_128_4}");
	auto s2mmC1_khdl = xrt::kernel(hw_ctx, "s2mm_32_128:{s2mm_32_128_1}");
	auto s2mmC2_khdl = xrt::kernel(hw_ctx, "s2mm_16_128:{s2mm_16_128_1}");

	//////////////////////////////////////////
    // Input and Output memories Allocation
	// Use kernel.group_id(0) to select the correct DDR memory group.
	// Hardcoding group 0 may map to a bank marked "Bank Used: No" on some
	// platforms, causing a runtime exception at set_arg.
	//////////////////////////////////////////
    auto A1_bohdl = xrt::bo(hw_ctx, pl_bramA_size_in_bytes, mm2sA1_khdl.group_id(0));
	auto A1_bomapped = A1_bohdl.map<int8_t*>();
    load_data_file<int8_t>("data/inputA_128.txt",A1_bomapped,INPUT_SIZEA);
	printf("A1 Input memory virtual addr 0x%px\n", A1_bomapped);

	auto B1_bohdl = xrt::bo(hw_ctx, pl_bramB_size_in_bytes, mm2sB1_khdl.group_id(0));
	auto B1_bomapped = B1_bohdl.map<int8_t*>();
    load_data_file<int8_t>("data/inputB_128.txt",B1_bomapped,INPUT_SIZEB);
	printf("B1 Input memory virtual addr 0x%px\n", B1_bomapped);

	auto A2_bohdl = xrt::bo(hw_ctx, pl_bramA_size_in_bytes, mm2sA2_khdl.group_id(0));
	auto A2_bomapped = A2_bohdl.map<int8_t*>();
    load_data_file<int8_t>("data/inputA_128.txt",A2_bomapped,INPUT_SIZEA);
	printf("A2 Input memory virtual addr 0x%px\n", A2_bomapped);

	auto B2_bohdl = xrt::bo(hw_ctx, pl_bramB_size_in_bytes, mm2sB2_khdl.group_id(0));
	auto B2_bomapped = B2_bohdl.map<int8_t*>();
    load_data_file<int8_t>("data/inputB_128.txt",B2_bomapped,INPUT_SIZEB);
	printf("B2 Input memory virtual addr 0x%px\n", B2_bomapped);

	auto C1_bohdl = xrt::bo(hw_ctx, pl_bramC1_size_in_bytes, s2mmC1_khdl.group_id(0));
	auto C1_bomapped = C1_bohdl.map<int32_t*>();
	memset(C1_bomapped, 0xABCDEF00, pl_bramC1_size_in_bytes);
	printf("C1 Output memory virtual addr 0x%px\n", C1_bomapped);

	auto C2_bohdl = xrt::bo(hw_ctx, pl_bramC2_size_in_bytes, s2mmC2_khdl.group_id(0));
	auto C2_bomapped = C2_bohdl.map<int16_t*>();
	memset(C2_bomapped, 0xABCDEF00, pl_bramC2_size_in_bytes);
	printf("C2 Output memory virtual addr 0x%px\n", C2_bomapped);

    int host_ref_32b[OUTPUT_SIZEC];
    load_data_file<int>("data/outputC_ref_128_32b.txt",host_ref_32b,OUTPUT_SIZEC);

    int host_ref_16b[OUTPUT_SIZEC];
    load_data_file<int>("data/outputC_ref_128_16b.txt",host_ref_16b,OUTPUT_SIZEC);

	////////////////////////////////////////////////////////
	// Start kernels
	///////////////////////////////////////////////////////

    int NTransactionsA = INPUT_SIZEA*DATAIN_NBYTES_8/PLIO_NBYTES;
    int NTransactionsB = INPUT_SIZEB*DATAIN_NBYTES_8/PLIO_NBYTES;
    int PacketSizeA = NTransactionsA/NITER;
    int PacketSizeB = NTransactionsB/NITER;

    auto mm2sA1_rhdl = mm2sA1_khdl(A1_bohdl, nullptr, NTransactionsA, PacketSizeA);
    auto mm2sB1_rhdl = mm2sB1_khdl(B1_bohdl, nullptr, NTransactionsB, PacketSizeB);
    auto mm2sA2_rhdl = mm2sA2_khdl(A2_bohdl, nullptr, NTransactionsA, PacketSizeA);
    auto mm2sB2_rhdl = mm2sB2_khdl(B2_bohdl, nullptr, NTransactionsB, PacketSizeB);

    int NTransactionsC1 = OUTPUT_SIZEC*DATAOUT_NBYTES_32/PLIO_NBYTES;
    int NTransactionsC2 = OUTPUT_SIZEC*DATAOUT_NBYTES_16/PLIO_NBYTES;
    auto s2mmC1_rhdl = s2mmC1_khdl(C1_bohdl, nullptr, NTransactionsC1);
    auto s2mmC2_rhdl = s2mmC2_khdl(C2_bohdl, nullptr, NTransactionsC2);

    std::cout << "Number of Transactions for A : " << NTransactionsA  <<  std::endl;
    std::cout << "Packet Size for A            : " << PacketSizeA  <<  std::endl;
    std::cout << "Number of Transactions for B : " << NTransactionsB  <<  std::endl;
    std::cout << "PacketSize for B             : " << PacketSizeB  <<  std::endl;
    std::cout << "Number of Transactions for C1: " << NTransactionsC1  <<  std::endl;
    std::cout << "Number of Transactions for C2: " << NTransactionsC2  <<  std::endl;


    // kernel_run_status("mm2s A1",mm2sA1_rhdl);
    // kernel_run_status("mm2s B1",mm2sB1_rhdl);
    // kernel_run_status("mm2s A2",mm2sA2_rhdl);
    // kernel_run_status("mm2s B2",mm2sB2_rhdl);
    // kernel_run_status("s2mm C1",s2mmC1_rhdl);
    // kernel_run_status("s2mm C2",s2mmC2_rhdl);

    // wait_status("mm2s A1",mm2sA1_rhdl);
    // wait_status("mm2s B1",mm2sB1_rhdl);
    // wait_status("mm2s A2",mm2sA2_rhdl);
    // wait_status("mm2s B2",mm2sB2_rhdl);
    // wait_status("s2mm C1",s2mmC1_rhdl);
    // wait_status("s2mm C2",s2mmC2_rhdl);

	//////////////////////////////////////////
	// graph execution for AIE
	//////////////////////////////////////////

	//Obtains the graph handle from the XCLBIN that is loaded into the device
	auto cghdl = xrt::graph(hw_ctx, "GMatMult");
    // cghdl.reset();

    std::cout << "Graph run: " << NITER  <<  std::endl;
    for(int iter=0;iter<NITER;iter++)
    {
        std::cout << "Iteration " << iter  <<  " starts..." << std::endl;
	    cghdl.run(1);
        std::cout << "Should be running. Entering wait state..." <<  std::endl;

        // if(iter==0)
        // {
        //     wait_status("s2mm C1",s2mmC1_rhdl);
        //     wait_status("s2mm C2",s2mmC2_rhdl);
        // }
        cghdl.wait();
        std::cout << "Iteration ended correctly." <<  std::endl <<  std::endl;
    }

	//Graph end
    cghdl.end(0);
	printf("graph end\n");

	//////////////////////////////////////////
	// wait for mm2s done
	//////////////////////////////////////////

	auto state_mm2sA1 = mm2sA1_rhdl.wait();
	std::cout << "mm2s (A1) completed with status(" << state_mm2sA1 << ")\n";
	auto state_mm2sB1 = mm2sB1_rhdl.wait();
	std::cout << "mm2s (B1) completed with status(" << state_mm2sB1 << ")\n";
	auto state_mm2sA2 = mm2sA2_rhdl.wait();
	std::cout << "mm2s (A2) completed with status(" << state_mm2sA2 << ")\n";
	auto state_mm2sB2 = mm2sB2_rhdl.wait();
	std::cout << "mm2s (B2) completed with status(" << state_mm2sB2 << ")\n";

    auto state_s2mmC1 = s2mmC1_rhdl.wait();
    std::cout << "s2mm (C1) completed with status(" << state_s2mmC1 << ")\n";
    auto state_s2mmC2 = s2mmC2_rhdl.wait();
    std::cout << "s2mm (C2) completed with status(" << state_s2mmC2 << ")\n";


    int match1 = 0;
    std::cout<< "Compare Ref and aie in 32b" << std::endl;
    for (int i = 0; i < OUTPUT_SIZEC; i++) {
        if(C1_bomapped[i]!=host_ref_32b[i]){
            if(match1<=10)
                std::cout<<"ERROR: i="<<i<<" ref="<< host_ref_32b[i]
                    << "        out=" << C1_bomapped[i]<<std::endl;
            if(match1==11)
                std::cout << "..." << std::endl;
            match1++;
        }
    }


    int match2 = 0;
    std::cout<< "Compare Ref and aie in 16b" << std::endl;
    for (int i = 0; i < OUTPUT_SIZEC; i++) {
        if(C2_bomapped[i]!=host_ref_16b[i]){
            if(match2<=10)
                std::cout<<"ERROR: i="<<i<<" ref="<< host_ref_16b[i]
                    << "        out=" << C2_bomapped[i]<<std::endl;
            if(match2==11)
                std::cout << "..." << std::endl;
            match2++;
        }
    }


    return ((match1+match2)!=0);
}

int main(int argc, char* argv[])
{
    try {
        auto match = run(argc, argv);
        std::cout << "TEST " << (match ? "FAILED" : "PASSED") << "\n";
        return (match ? EXIT_FAILURE :  EXIT_SUCCESS);
    }
    catch (std::exception const& e) {
        std::cout << "Exception: " << e.what() << "\n";
        std::cout << "FAILED TEST\n";
        return 1;
    }
}
