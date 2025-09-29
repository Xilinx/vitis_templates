
#include "graph.h"
#include <fstream>

using namespace adf;

myGraph gr;

int main(int argc, char ** argv)
{
    int32* inputArray = (int32*)GMIO::malloc(256*sizeof(int32));
    int32* outputArray = (int32*)GMIO::malloc(256*sizeof(int32));

    for (int i=0; i<256; i++)
    {
       inputArray[i] = i+1;
    }

    std::ofstream ofsBw;
    std::ofstream ofsData;

#ifdef __AIESIM__
    system("mkdir -p aiesimulator_output/data");
    ofsBw.open("aiesimulator_output/data/outputBandwidth.txt", std::ofstream::out | std::ofstream::trunc);
    ofsData.open("aiesimulator_output/data/outputData.txt", std::ofstream::out | std::ofstream::trunc);
#else
    system("mkdir -p x86simulator_output/data");
    ofsBw.open("x86simulator_output/data/outputBandwidth.txt", std::ofstream::out | std::ofstream::trunc);
    ofsData.open("x86simulator_output/data/outputData.txt", std::ofstream::out | std::ofstream::trunc);
#endif

    gr.init();
    event::handle handle = event::start_profiling(gr.out, event::io_total_stream_running_to_idle_cycles);

    gr.in.gm2aie_nb(&inputArray[0], 256*sizeof(int32));
    gr.out.aie2gm_nb(&outputArray[0], 256*sizeof(int32));

    gr.run(8);

    gr.out.wait();

    long long cycle_count = event::read_profiling(handle);
    double bandwidth = (double)256*sizeof(int32) / (cycle_count*1e-9); //Byte per second
    std::cout<<"Bandwidth "<<std::dec<<bandwidth<<" B/s"<<std::endl;
    ofsBw<<bandwidth<<std::endl;

    event::stop_profiling(handle);

    for (int i=0; i<256; i++)
    {
       ofsData<<outputArray[i]<<std::endl;
    }

    gr.end();

    ofsBw.close();
    ofsData.close();
    GMIO::free(inputArray);
    GMIO::free(outputArray);
    return 0;
}

