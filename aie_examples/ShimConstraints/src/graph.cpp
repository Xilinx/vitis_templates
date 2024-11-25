
#include "kernels.h"
#include "graph.h"

indep_nodes_graph graph0;

int main(int argc, char ** argv) {
   graph0.init();
   graph0.run(4);
   graph0.end();
   return 0;
}
