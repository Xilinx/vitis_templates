
#include "project.h"

chainedGraph mygraph;

int main(void) {
  mygraph.init();
  mygraph.run(4);
  for(int i = 0; i<4; i++) {
    int val[NUM_SAMPLES];
    mygraph.read(mygraph.first.inout[0], val, NUM_SAMPLES);
    mygraph.update(mygraph.second.in[0], val, NUM_SAMPLES);
  }
  mygraph.end();
  return 0;
}
