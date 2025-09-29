#include <adf.h>
#include <graph.h>

using namespace adf;


MEPL128BitClass gMePl;

int main(int argc, char ** argv) 
{ 
  gMePl.init();
  gMePl.run(4);
  gMePl.end();
  return 0;
}
