
#include <adf.h>
#include "kernels.h"
#include "project.h"

using namespace adf;

simpleGraph mygraph;

int main(void) {
  int value;
  mygraph.init();
  mygraph.run(4);
  value = 1;
  mygraph.update(mygraph.ctrl, value);
  value = 0;
  mygraph.update(mygraph.ctrl, value);
  value = 0;
  mygraph.update(mygraph.ctrl, value);
  value = 1;
  mygraph.update(mygraph.ctrl, value);
  mygraph.end();
  return 0;
}
