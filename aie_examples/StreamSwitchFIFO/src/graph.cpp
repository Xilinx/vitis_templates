
#include "graph.h"

using namespace adf;

StreamSwitchFIFO fifo_graph;

int main(int argc, char ** argv) { 

  fifo_graph.init();
  fifo_graph.run(4);
  fifo_graph.end();
  return 0;

}

