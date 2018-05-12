#include "Animal.hpp"
#include "Simulation.hpp"

namespace AnimalSimulation {

  Animal::Animal(float x, float y, float th) : theta(th), strength(10), lifespan(0), maxSpeed(0.001), 
    maxEnergy(30.0), timerFrequency(2*PI*0.1), birthDelay(ANIMAL_MEAN_BIRTH_DELAY), timeSinceLastBirth(0)
  {
    position.x = x;
    position.y = y;

    // Parameters and outputs arrays
    parameters = new float[NUMBER_OF_PARAMETERS];
    outputs    = new float[NUMBER_OF_OUTPUTS];

    // Create brain
    // Create layers
    int size = 2;
    layers = vector<int>(size);
    layers.at(0) = NUMBER_OF_PARAMETERS;
    layers.at(1) = NUMBER_OF_OUTPUTS;
    // Create vectors
    neurons = vector<Matrix>(size-1);
    biases  = vector<Matrix>(size-1);
    out     = vector<Matrix>(size-2);
    // Initialize layers
    for (int i=0; i<size-1; ++i) {
      neurons.at(i).resize(layers.at(i+1), layers.at(i));
      biases.at(i).resize(layers.at(i+1), 1);
      neurons.at(i).randomUniform( 1./sqrt(layers.at(i)) );
      biases.at(i).randomUniform();
    }
    for (int i=0; i<size-2; ++i) out.at(i).resize(layers.at(i+1), 1);

    // Set initial parameters
    parameters[0] = 30.0f; //  0 - energy
    parameters[1] = 0.5*maxSpeed; //  1 - velocity
    parameters[2] = 0.0f; //  2 - timer
    parameters[3] = 0.0f; //  3 - noise
    /*
    parameters[4] = 0.0f; //  3 - eyeF_R
    parameters[5] = 0.0f; //  4 - eyeF_G
    parameters[6] = 0.0f; //  5 - eyeF_B
    parameters[7] = 0.0f; //  6 - eyeR_R
    parameters[8] = 0.0f; //  7 - eyeR_G
    parameters[9] = 0.0f; //  8 - eyeR_B
    */
    

    // Set initial output
    outputs[0] = 0.0f; //  0 - d(theta)/dt
    outputs[1] = 0.0f; //  1 - dv/dt
    outputs[2] = 1.0f; //  2 - colorR
    outputs[3] = 0.0f; //  3 - colorG
    outputs[4] = 0.0f; //  4 - colorB

    // Make sure layers is at least 2
    if (layers.size()<2) throw BadAnimalBrain();
  };

  Animal::Animal(const Animal& a) : strength(a.strength), lifespan(0), maxSpeed(a.maxSpeed), maxEnergy(a.maxEnergy), 
    timerFrequency(a.timerFrequency), birthDelay(a.birthDelay), timeSinceLastBirth(0)
  {
    theta    = a.theta;
    position = a.position;

    // Parameters and outputs arrays
    parameters = new float[NUMBER_OF_PARAMETERS];
    outputs    = new float[NUMBER_OF_OUTPUTS];

    // Create brain
    // Create layers
    int size = 2;
    layers = a.layers;
    // Create vectors
    neurons = a.neurons;
    biases = a.biases;
    out = a.out;

    // Set initial parameters
    parameters[0] = 30.0f; //  0 - energy
    parameters[1] = 0.5*maxSpeed; //  1 - velocity
    parameters[2] = 0.0f; //  2 - timer
    parameters[3] = 0.0f; //  3 - noise
    
    // Set initial output
    outputs[0] = 0.0f; //  0 - d(theta)/dt
    outputs[1] = 0.0f; //  1 - dv/dt
    outputs[2] = 1.0f; //  2 - colorR
    outputs[3] = 0.0f; //  3 - colorG
    outputs[4] = 0.0f; //  4 - colorB
  }

  Animal::~Animal() {
    delete [] parameters;
    delete [] outputs;
  }

  bool Animal::update(float dt, Simulation* simulation) {
    if (parameters[0]<0) return true; // Is dead

    // Set the noise input
    parameters[3] = randNormal(); // Multiply by sqrt(dt) ?

    /*
        For example:
        layer.size() = 4
      
      0         1         2         3
        ->W0->     ->W1->    ->W2->
      P         O0        O1        Out
                b0        b1        b2
    */

    // Do "thinking"
    int size = layers.size();
    if (size>2) {
      multiply(neurons.at(0), parameters, out.at(0));
      out.at(0).apply(sigmoid);
      for (int i=1; i<size-2; ++i) {
        multiply(neurons.at(i), out.at(i-1), out.at(i));
        NTplusEqUnsafe(out.at(i), biases.at(i));
        out.at(i).apply(sigmoid);
      }
      multiply(neurons.at(size-2), out.at(size-3), outputs);
      NTplusEqUnsafe(outputs, biases.at(size-2));
      for (int i=0; i<5; ++i) outputs[i] = sigmoid(outputs[i]);
    }
    else { // size == 2
      multiply(neurons.at(0), parameters, outputs);
      NTplusEqUnsafe(outputs, biases.at(0));
      for (int i=0; i<5; ++i) outputs[i] = sigmoid(outputs[i]);
    }
    
    // Perturb angle, update x, y
    theta += outputs[0] * dt;

    // Update velocity
    parameters[1] += outputs[1] * dt; 
    // Keep velocity in bounds
    if      (parameters[1]>maxSpeed)  parameters[1] = maxSpeed;
    else if (parameters[1]<-maxSpeed) parameters[1] = -maxSpeed;

    // Update position
    position.x += parameters[1] * cosf(theta);
    position.y += parameters[1] * sinf(theta);
    // Keep in bounds
    simulation->wrap(position);

    if (position.x>1.f) position.x = -1.f;
    else if (position.x<-1.f) position.x = 1.f;
    if (position.y>1.f) position.y = -1.f;
    else if (position.y<-1.f) position.y = 1.f;

    // Subtract energy
    parameters[0] -= (MIN_ENERGY_LOSS + fabs(parameters[1])*RUN_ENERGY_LOSS) * dt;

    // Possibly have children - if enough time has gone by and the animal has enough energy
    if (timeSinceLastBirth>birthDelay && parameters[0]>0.2*maxEnergy) {
      Animal *a = createOffspring();
      simulation->addAnimal(a);
      parameters[0] -= 0.2*maxEnergy;
      timeSinceLastBirth = 0;
    }

    // Lifespan and timers
    lifespan += dt; // Update lifespan
    timeSinceLastBirth += dt;
    parameters[2] = sinf(timerFrequency*lifespan); // Set timer

    // If the animal is out of energy, it dies
    if (parameters[1] < 0) return true;
    // Not dead
    return false;
  }

  void Animal::draw() {
    #if USE_OPENGL==1
    //drawBall(position, 0.02, 1, 1, 1);
    drawTriangle(position, theta, 0.02, outputs[2], outputs[3], outputs[4]);
    #endif // #if USE_OPENGL==1
  }

  vec2 Animal::getPosition() {
    return position;
  }

  Animal* Animal::randomAnimal() {
    Animal *animal         = new Animal(0,0,2*PI*drand48());
    animal->strength       = ANIMAL_MEAN_STRENGTH + 3*randNormal();
    animal->maxEnergy      = animal->parameters[0] = ANIMAL_MEAN_MAX_ENERGY + 5*randNormal();
    animal->timerFrequency = ANIMAL_MEAN_FREQUENCY + 0.2*randNormal();
  }

    // Create an offspring
  Animal* Animal::createOffspring() {
    Animal *child = new Animal(*this);
    // Change angle
    child->theta = 2*PI*drand48();
    // Mutate attributes
    child->strength  += ANIMAL_MUTATION_STRENGTH*randNormal();
    child->maxEnergy += ANIMAL_MUTATION_STRENGTH*randNormal();
    child->timerFrequency += ANIMAL_MUTATION_STRENGTH*randNormal();
    child->parameters[0] = 0.25*child->maxEnergy;
    // Mutate brain
    for (int i=0; i<child->neurons.size(); ++i) {
      Matrix &n = child->neurons.at(i);
      Matrix perturbation(n.getRows(), n.getCols());
      perturbation.randomNormal();
      NTplusEqUnsafe(n, perturbation);
    }
    for (int i=0; i<child->biases.size(); ++i) {
      Matrix &b = child->biases.at(i);
      Matrix perturbation(b.getRows(), b.getCols());
      perturbation.randomNormal();
      NTplusEqUnsafe(b, perturbation);
    }
    // Return the animal
    return child;
  }

}
