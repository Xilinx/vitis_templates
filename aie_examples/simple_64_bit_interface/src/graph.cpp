#include <adf.h>
#include "graph.h"

MEPL64BitInterface MePlGraph;

int main(int argc, char **argv) 
{
  MePlGraph.init();
  MePlGraph.run(4);
  MePlGraph.end();
return 0; }
