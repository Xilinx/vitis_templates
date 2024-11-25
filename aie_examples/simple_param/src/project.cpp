
#include "project.h"

parameterGraph mygraph;

int main(void) {
  int value=1;
  mygraph.init();
  mygraph.run(4);
  for (int i=0; i<4; i++)
        mygraph.update(mygraph.select_value, value);
  mygraph.end();
  return 0;
}
