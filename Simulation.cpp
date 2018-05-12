#include "Simulation.hpp"

namespace AnimalSimulation {

  Simulation::Simulation() : dt(0.01), refreshRate(30), fullScreenMode(false), windowWidth(640), 
    windowHeight(480), windowPosX(50), windowPosY(50), width(1.0f), height(1.0f), time(0.0f), 
    timeSinceLastDisplay(1.0f), realTime(true), displayRate(1.)
  {
    last_tick = high_resolution_clock::now();

    cameraX = 0.5*width;
    cameraY = 0.5*height;

    // Put in some animals
    Animal *a = new Animal(0,0,0);
    animals.push_back(a);
    for (int i=1; i<10; ++i)
      animals.push_back(new Animal(0,0,0)); // Animal::createOffspring(a));

    // Put in some plants
    for (int i=0; i<10; ++i)
      plants.push_back(new Plant(drand48(), drand48()));
  }

  Simulation::~Simulation() {
    for (auto a : animals) delete a;
    animals.clear();
    for (auto p : plants) delete p;
    plants.clear();
  }

  void Simulation::initialize(int argc, char** argv) {
    // Initialize OpenGL functions
    #if USE_OPENGL==1
      // Initialize GLUT
      glutInit(&argc, argv);
      // Use double buffering
      glutInitDisplayMode(GLUT_DOUBLE); 
      // Window initial position and size
      glutInitWindowSize(640, 480); 
      glutInitWindowPosition(50, 50);
      // Name the window
      glutCreateWindow("Animal Simulation - v1.0");  // Create window with the given title
      // Give GLUT our functions
      glutDisplayFunc(this->display);       // Register callback handler for window re-paint event
      glutReshapeFunc(this->reshape);       // Register callback handler for window re-size event
      glutIdleFunc   (this->calculation);   // Register callback handler if no other event
      glutTimerFunc(0, this->timer, 0);     // First timer call immediately
      glutKeyboardFunc(this->handleInput);        // Register callback handler for special-key event
      glutSpecialFunc(this->handleSpecialInput);  // Register callback handler for special-key event
      glutMouseFunc(this->handleMouseInput);      // Register callback handler for mouse event
      
      // Set "clearing" or background color
      glClearColor(0.0f, 0.8f, 0.0f, 1.0f);
    #endif // #if USE_OPENGL==1
  }

  void Simulation::run() {
    #if USE_OPENGL==1
      // Enter the infinite event-processing loop
      glutMainLoop();
    #else
      while (true) {
        _calculation();
      }
    #endif // #if USE_OPENGL==1
  }

  void Simulation::wrap(vec2& position) {
    // **** This will need to be retooled ***
    if (position.x>width) position.x = -width;
    else if (position.x<-width) position.x = width;
    if (position.y>height) position.y = -height;
    else if (position.y<-height) position.y = height;
  }

  void Simulation::addAnimal(Animal *animal) {
    newAnimals.push_back(animal);
  }

  void Simulation::update(float dt) {
    // Get rid of dead plants and animals
    for (auto a : deadAnimals) {
      Animal *A = *a;
      animals.erase(a);
      delete A;
    }
    deadAnimals.clear();
    for (auto p : deadPlants) {
      delete *p;
      plants.erase(p);
    }
    deadPlants.clear();
    // Add new plants and animals
    for (auto a : newAnimals) {
      animals.push_back(a);
    }
    newAnimals.clear();
    for (auto p : newPlants) plants.push_back(p);
    newPlants.clear();

    // Update timers
    timeSinceLastDisplay += dt;
    time += dt;
  }

#if USE_OPENGL==1

  void Simulation::calculation() {
    if (simulation) simulation->_calculation();    
  }

  void Simulation::display() {
    if (simulation) simulation->_display();
  }

  void Simulation::reshape(int width, int height) {
    if (simulation) simulation->_reshape(width, height);
  }

  void Simulation::timer(int value) {
    if (simulation) simulation->_timer(value);
  }

  void Simulation::handleInput(unsigned char key, int x, int y) {
    if (simulation) simulation->_handleInput(key, x, y);
  }

  void Simulation::handleSpecialInput(int key, int x, int y) {
    if (simulation) simulation->_handleSpecialInput(key, x, y);
  }

  void Simulation::handleMouseInput(int button, int state, int x, int y) {
    if (simulation) simulation->_handleMouseInput(button, state, x, y);
  }

#endif // #if USE_OPENGL==1

  void Simulation::_calculation() {
    // Check if anything needs to be done
    #if USE_OPENGL==1
    if ( timeSinceLastDisplay < 1e-3*displayRate*refreshRate || !realTime ) {
    #endif // #if USE_OPENGL==1
      // Update the animals
      for (auto a = animals.begin(); a!=animals.end(); ++a)
        if ( (*a)->update(dt, this) )
          a = animals.erase(a);
      // Update the plants
      for (auto p = plants.begin(); p!=plants.end(); ++p)
        if ( (*p)->update(dt, this) )
	  p = plants.erase(p);
      // Update the simulation
      update(dt);
    #if USE_OPENGL==1
    }
    #endif // #if USE_OPENGL==1
  }

#if OpenGL==1

  void Simulation::_display() {
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);   

    // Draw the boundary
    glBegin(GL_LINE_LOOP);
      // The boundary has white lines
      glColor3f(1.0f, 1.0f, 1.0f);
      // The rectangle
      glVertex2f(-1.0f, 1.0f);
      glVertex2f(-1.0f, -1.0f);
      glVertex2f(1.0f, -1.0f);
      glVertex2f(1.0f, 1.0f);
    glEnd();

    // Draw all the plants (under the animals)
    for (auto p : plants) p->draw();
    // Draw all the animals
    for (auto a : animals) a->draw();
   
    // Double buffered - swap the front and back buffers
    glutSwapBuffers();   

    // Reset timer
    timeSinceLastDisplay = 0.0;
  }

  void Simulation::_reshape(int rwidth, int rheight) {
    // To prevent divide by 0
    rheight = (rheight==0 ? 1 : rheight);
    // Compute aspect ratio of the new window
    GLfloat aspect = (GLfloat)rwidth / (GLfloat)rheight;
   
    // Set the viewport to cover the new window
    glViewport(0, 0, rwidth, rheight);
   
    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();
    if (rwidth >= rheight) {
      // aspect >= 1, set the height from -1 to 1, with larger width
      gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } 
    else {
      // aspect < 1, set the width to -1 to 1, with larger height
      gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
  }

  void Simulation::_timer(int value) {
    const int refreshMills = 30;
    // Post re-paint request to activate display()
    glutPostRedisplay();
    // Call this function ([Timer]) again after [refreshMills] milliseconds
    glutTimerFunc(refreshMills, timer, 0);
  }

  void Simulation::_handleInput(unsigned char key, int x, int y) {
    switch (key) {
      case '=':
        displayRate *= 2.0;
        break;
      case '-':
        displayRate /= 2.0;
        break;
      case 27: // Esc
        exit(0);
        break;
    }
  }

  void Simulation::_handleSpecialInput(int key, int x, int y) {
    switch (key) {
      case GLUT_KEY_UP:    // F1: Toggle between full-screen and windowed mode
        fullScreenMode = !fullScreenMode;         // Toggle state
        if (fullScreenMode) {                     // Full-screen mode
          windowPosX   = glutGet(GLUT_WINDOW_X);  // Save parameters for restoring later
          windowPosY   = glutGet(GLUT_WINDOW_Y);
          windowWidth  = glutGet(GLUT_WINDOW_WIDTH);
          windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
          glutFullScreen();                       // Switch into full screen
        } 
        else {                                         // Windowed mode
          // glutReshapeWindow(windowWidth, windowHeight); // Switch into windowed mode
          glutReshapeWindow(640, 480); // Switch into windowed mode
          // glutPositionWindow(windowPosX, windowPosX);   // Position top-left corner
          glutPositionWindow(50, 50);   // Position top-left corner
        }
        break;
    }    
  }

  void Simulation::_handleMouseInput(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { // Pause/resume
      //
    }
  }

  #endif // #if USE_OPENGL==1

}
