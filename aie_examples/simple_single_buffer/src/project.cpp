
#include "project.h"

simpleGraph mygraph;

int main(void) {
  mygraph.init();
  mygraph.run(4);
  mygraph.end();
  return 0;
}
