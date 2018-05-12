CC=g++
CSTD= -std=c++14
FLAGS= -w -g

FRAMEWORK = -framework OpenGL -framework GLUT
LIBPATH += -L"/System/Library/Frameworks/OpenGL.framework/Libraries"
LIBRARIES = -lGL -lGLU -lglut -lm -lobjc -lstdc++ 

targets = driver
objects = Simulation.o Animal.o Plant.o Matrix.o

all: $(targets)

# Executables
driver: driver.o $(objects)
	$(CC) $(CSTD) $(FRAMEWORK) $(ARGS) -o $@ $^ $(LIBPATH) $(LIBRARIES)

# Object files
%.o : %.cpp
	$(CC) -c $(CSTD) $(FLAGS) $^

# Clean
.PHONY : clean
clean :
	rm -f *.o *.optrpt $(targets)