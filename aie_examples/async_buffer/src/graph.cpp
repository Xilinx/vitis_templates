
#include <adf.h>
#include "graph.h"


mygraph gr;

int main(int argc, char ** argv) { 

  gr.init();
  gr.run(4);
  gr.end();
  return 0;
}

