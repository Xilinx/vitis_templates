
#include "graph.h"

#define NUM_CORES 2
// <int COLS, int ROWS, int STARTCOL, int STARTROW>
indep_nodes_graph<1,2,0,0> g;

int main(int argc, char ** argv) {
   g.init();
   g.run(4);
   g.end();
   return 0;
}
