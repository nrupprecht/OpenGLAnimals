// Include Simulation
#include "Simulation.hpp"

// Declare exter variables
namespace AnimalSimulation {
  Simulation *simulation = nullptr;
}

std::mt19937 generator = std::mt19937();
std::normal_distribution<double> normal_dist = std::normal_distribution<double>(0., 1.);

using namespace AnimalSimulation;
 
/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
  // Seed random number generator
  seedGenerator();

  /*
  Animal *a = new Animal(0,0,0);
  Animal *b = Animal::createOffspring(a);
  return 0;
  */

  // Initialize the simulation
  simulation = new Simulation;
  simulation->initialize(argc, argv);

  //try {
  simulation->run();
    /*
  }
  catch(...) {
    cout << "An error has occured.\n";
  }
  */

  return 0;
}