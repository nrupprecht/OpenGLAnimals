#include "Plant.hpp"

namespace AnimalSimulation {

  Plant::Plant(float x, float y) : energy(1), maxEnergy(50), lifespan(0), colorR(0), colorG(0.8), colorB(0.5) {
    position.x = x;
    position.y = y;
  }

  bool Plant::update(float dt, Simulation* simulation) {
    // Update energy and life
    if (energy < maxEnergy) energy += PLANT_GROWTH_RATE*dt;
    lifespan += dt;

    // Send out seeds

    // Check if dead
    if (energy<0) return true;
    // Not dead
    return false;
  }

  void Plant::draw() {
    #if USE_OPENGL==1
    float radius = 0.0005*energy + 0.01;
    drawBall(position, radius, colorR, colorG, colorB);
    drawCircle(position, radius, 0, 0, 0); // Black outline
    #endif // #if USE_OPENGL==1
  }

}