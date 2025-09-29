#include "graph.h"

using namespace adf;

mygraph g;

int main()
{
    g.init();
    g.run(32);
    g.end();
    return 0;
}
