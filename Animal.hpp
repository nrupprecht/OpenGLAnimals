#ifndef __ANIMAL_HPP__
#define __ANIMAL_HPP__

#include "OpenGLUtility.hpp"
#include "Matrix.hpp"


namespace AnimalSimulation {

  // Forward declaration to simulation
  class Simulation;

  // Sigmoid function
  inline double sigmoid(double x) {
    return 1.0/(1.0+exp(-x));
  }

  // Animal constants
  const int NUMBER_OF_PARAMETERS = 4;
  const int NUMBER_OF_OUTPUTS = 5;
  const float MIN_ENERGY_LOSS = 1.0f;
  const float RUN_ENERGY_LOSS = 1.0f;
  const float ANIMAL_MEAN_STRENGTH = 10.0f;
  const float ANIMAL_MEAN_MAX_ENERGY = 30.0f;
  const float ANIMAL_MEAN_FREQUENCY = 1.0f;
  const float ANIMAL_MUTATION_STRENGTH = 0.1f;
  const float ANIMAL_MEAN_BIRTH_DELAY = 20.0f;

  class Animal {
  public:
    // Construct animal with position and angle
    Animal(float, float, float);

    // Construct an animal of a duplicate type
    Animal(const Animal&);

    // Destructor
    ~Animal();

    // Have the animal make decisions and update itself - pass in dt and a pointer to the simulation - if true, the animal has died
    bool update(float, Simulation*);

    // Ask the animal to draw itself
    void draw();

    // Accessors
    vec2 getPosition();

    // Create a random animal
    static Animal* randomAnimal();

    // Create an offspring
    static Animal* createOffspring(Animal*);

    // Exception classes
    class BadAnimalBrain {};

  private:
    // Location variables
    vec2 position;  // Position of the animal
    float theta; // Direction of the animal
    float strength; // Strength of the animal
    float lifespan; // How long the animal has lived for
    float maxSpeed; // Maximum possible speed
    float maxEnergy; // Maximum energy an animal can store
    float timerFrequency; // Angular frequency of the timer
    float birthDelay; // Time to wait between births
    float timeSinceLastBirth; // Time since the animal last gave birth

    // Other variables - variables that are inputs to the animal's mind are stored as vectors
    float *parameters;
    //  0 - energy
    //  1 - velocity
    //  2 - timer
    //  3 - noise
    //  4 - eyeF_R
    //  5 - eyeF_G
    //  6 - eyeF_B
    //  7 - eyeR_R
    //  8 - eyeR_G
    //  9 - eyeR_B

    // Outputs of the mind of the animal
    float *outputs;
    //  0 - d(theta)/dt
    //  1 - dv/dt
    //  2 - colorR
    //  3 - colorG
    //  4 - colorB

    // The animal's brain
    vector<Matrix> neurons;    // The weights
    vector<Matrix> biases;     // The biases
    vector<Matrix> out;        // Intermediate brain outputs
    vector<int> layers; // The sizes of layers

  };
}

#endif // __ANIMAL_HPP__