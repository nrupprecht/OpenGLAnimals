#ifndef __SIMULATION_HPP__
#define __SIMULATION_HPP__

#include "Animal.hpp"
#include "Plant.hpp"

// Global variables

namespace AnimalSimulation {

  class Simulation {
  public:
    // Default constructor
    Simulation();

    // Destructor
    ~Simulation();

    // Initialize simulation and opengl
    void initialize(int, char**);

    // Run the simulation
    void run();

    // Keep position in bounds
    void wrap(vec2&);

    // Add a new animal to the simulation
    void addAnimal(Animal*);

  private:
    // Helper functione
    inline void update(float);

    #if USE_OPENGL==1
    // Static OpenGL functions
    static void calculation();
    static void display();
    static void reshape(int, int);
    static void timer(int);
    static void handleInput(unsigned char, int, int);
    static void handleSpecialInput(int, int, int);
    static void handleMouseInput(int button, int state, int x, int y);
    #endif // #if USE_OPENGL==1

    // Specific OpenGL functions
    void _calculation();
    #if USE_OPENGL==1
    void _display();
    void _reshape(int, int);
    void _timer(int);
    void _handleInput(unsigned char, int, int);
    void _handleSpecialInput(int, int, int);
    void _handleMouseInput(int button, int state, int x, int y); 
    #endif // #if USE_OPENGL==1

    // Simulation constants
    float width, height; // Width and height of the simulation

    // Simulation dt
    float dt;    // Time step
    float time;    // How much time has been simulated
    float timeSinceLastDisplay; // How much simulation time has gone by since the last time the the simulation was displayed
    bool realTime; // If true, try to run in real time, if false, run as fast as possible
    float displayRate; // How many simulation seconds per real second should the display try to run at
    // OpenGL refresh rate (number of milliseconds)
    int refreshRate;

    // Last time calculations were done
    std::chrono::high_resolution_clock::time_point last_tick;

    // List of pointers to all the animals in the simulation
    list<Animal*> animals;
    list<Animal*> newAnimals; // Animals to be added
    vector<list<Animal*>::iterator> deadAnimals; // To remove from the animals list

    // List of pointers to all the plants in the simulation
    list<Plant*> plants;
    list<Plant*> newPlants; // Plants to be added
    vector<list<Plant*>::iterator> deadPlants; // To remove from the plant list

    // Whether the window is fullscreen or not
    bool fullScreenMode;
    // Position of the center camera
    float cameraX, cameraY;
    // Windowed mode data
    int windowWidth, windowHeight;
    int windowPosX, windowPosY;
  };

  extern Simulation* simulation;

}
#endif // __SIMULATION_HPP__