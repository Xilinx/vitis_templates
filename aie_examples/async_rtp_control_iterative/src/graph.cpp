
#include "graph.h"
#include <fstream>

using namespace adf;

rtpGraph mygraph;

int main(void) {    
    int selector = 0;
    int rtpOut = 0;
    std::ofstream ofsRTP;

#ifndef __PTHREAD_API__
    system("exec mkdir -p aiesimulator_output/data");
    ofsRTP.open("aiesimulator_output/data/rtp_output.txt", std::ofstream::out | std::ofstream::trunc);
#else
    system("exec mkdir -p x86simulator_output/data");
    ofsRTP.open("x86simulator_output/data/rtp_output.txt", std::ofstream::out | std::ofstream::trunc);
#endif
  
    mygraph.init();
    mygraph.run(4);
    mygraph.update(mygraph.rtpInput, 2);
    mygraph.wait();
    mygraph.read(mygraph.rtpOutput, rtpOut);
    ofsRTP<<rtpOut<<std::endl;

    mygraph.update(mygraph.rtpInput, 3);
    mygraph.run(7);
    mygraph.wait();
    mygraph.read(mygraph.rtpOutput, rtpOut);
    ofsRTP<<rtpOut<<std::endl;

    mygraph.update(mygraph.rtpInput, 1);
    mygraph.run(3);
    mygraph.end();
    mygraph.read(mygraph.rtpOutput, rtpOut);
    ofsRTP<<rtpOut<<std::endl;

	ofsRTP.close();
  
    return 0;
}
