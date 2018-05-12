
#ifndef __PLANT_HPP__
#define __PLANT_HPP__

#include "OpenGLUtility.hpp"

namespace AnimalSimulation {

  // Forward declaration to simulation
  class Simulation;

  // Plant constants
  const float PLANT_GROWTH_RATE = 1.0f;

  class Plant {
  public:
    Plant(float, float);

    // Have the plant update itself
    bool update(float, Simulation*);

    // Ask the plant to draw itself
    void draw();

  private:
    vec2 position;   // Position of the plant
    float energy;    // Plants gain energy as they grow
    float maxEnergy; // Maximum energy this plant can have
    float lifespan;

    // Plant color
    float colorR, colorG, colorB;
  };

}
#endif // __PLANT_HPP__