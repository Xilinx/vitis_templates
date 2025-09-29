#include "project.h"
#include <fstream>
#include <cstdlib>

simpleGraph mygraph;

int main(int argc, char **argv) 
{
  volatile int inputFactor = 3; 
  int inputVector[NUM_SAMPLES] = {1,2,3,4,5,6,7,8};  
     
  int outputVector[NUM_SAMPLES];
  int outputFactor;
  
  std::ofstream ofsFactor;
  std::ofstream ofsVector;
    
  #ifndef __PTHREAD_API__
    system("mkdir -p aiesimulator_output/data");
    ofsFactor.open("aiesimulator_output/data/factor_output.txt", std::ofstream::out | std::ofstream::trunc);
    ofsVector.open("aiesimulator_output/data/vector_output.txt", std::ofstream::out | std::ofstream::trunc);
  #else
    system("mkdir -p x86simulator_output/data");
    ofsFactor.open("x86simulator_output/data/factor_output.txt", std::ofstream::out | std::ofstream::trunc);
    ofsVector.open("x86simulator_output/data/vector_output.txt", std::ofstream::out | std::ofstream::trunc);
  #endif 

  mygraph.init();
  mygraph.run(4);

  for (int i = 0; i < 4; i++)   
  {
    mygraph.update(mygraph.vectorInput, inputVector, NUM_SAMPLES);
    mygraph.update(mygraph.factorInput, inputFactor);
    mygraph.read(mygraph.vectorOutput, outputVector, NUM_SAMPLES);
    mygraph.read(mygraph.factorOutput, outputFactor);

    ofsFactor<<outputFactor<<std::endl;
    for (int i = 0; i < NUM_SAMPLES; i++)
      ofsVector<<outputVector[i]<<std::endl;
    //update inputVector and inputFactor
    inputFactor = outputFactor;
    for (int i = 0; i < NUM_SAMPLES; i++)
      inputVector[i] = outputVector[i];     
  }
    
  ofsFactor.close();
  ofsVector.close();
  	 
  mygraph.end();
  return 0;
} 
