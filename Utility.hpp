#ifndef __ANIMALS_OPENGL_UTILITY_HPP__
#define __ANIMALS_OPENGL_UTILITY_HPP__

// Constants
const float PI = 3.1415927;

// Standard streams and strings
#include <iostream>
using std::cout;
using std::endl;
#include <sstream>
using std::stringstream;
#include <ostream>
#include <string>
using std::string;

// Vector
#include <vector>
using std::vector;

// List
#include <list>
using std::list;

// Timing related things
#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

// high_resolution_clock::time_point
template<typename T> inline double time_span(T end, T start) {
  duration<double> span = duration_cast<duration<double>>(end-start);
  return span.count();
}

// Random number generators - define these in the main program
#include <random>
extern std::mt19937 generator;
extern std::normal_distribution<double> normal_dist; // std::normal_distribution<double>(0., 1.)

inline void seedGenerator() {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = std::mt19937(seed);
}

inline double randNormal() {
  return normal_dist(generator);
}

// Print an array
template<typename T> void print(T *data, int size, std::ostream& out = cout) {
  out << "{";
  for (int i=0; i<size; ++i) {
    out << data[i];
    if (i!=size-1) out << ",";
  }
  out << "}";
}

// Turn an array into a string
template<typename T> string toString(T *data, int size) {
  stringstream stream;
  stream << "{";
  for (int i=0; i<size; ++i) {
    stream << data[i];
    if (i!=size-1) stream << ",";
  }
  stream << "}";
  string str;
  stream >> str;
  return str;
}

#endif // __ANIMALS_OPENGL_UTILITY_HPP__