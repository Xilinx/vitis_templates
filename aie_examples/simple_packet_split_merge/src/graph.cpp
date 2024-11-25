

#include "graph.h"

ExplicitPacketSwitching g1;

int main(int argc, char ** argv) {
  g1.init(); 
  g1.run(4);
  g1.end();
  return 0;
}
