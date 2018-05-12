#ifndef __ANIMALS_OPENGL_OPENGL_UTILITY_HPP__
#define __ANIMALS_OPENGL_OPENGL_UTILITY_HPP__

/*
 * Useful reference: <https://www.ntu.edu.sg/home/ehchua/programming/opengl/CG_Introduction.html>
 *
 */

// Includes
#include "Utility.hpp"
#include "vec2d.hpp"

// Use OpenGL
#define USE_OPENGL 0

#if USE_OPENGL==1
// OpenGL headers
#include <GL/glew.h>
#include <GL/glut.h>

// Render a ball at coordinates (x,y) with color (R,G,B)
inline void drawBall(const GLfloat x, const GLfloat y, const GLfloat radius, const GLfloat R=1.f, const GLfloat G=1.f, const GLfloat B=1.f) {
  glBegin(GL_TRIANGLE_FAN);
  glColor3f(R, G, B);
  int numSegments = 100;
  GLfloat angle;
  for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
    angle = i * 2.0f * PI / numSegments;  // 360 deg for all segments
    glVertex2f(x + cosf(angle) * radius, y + sinf(angle) * radius);
  }
  glEnd();
}

inline void drawBall(const vec2 p, const GLfloat radius, const GLfloat R=1.f, const GLfloat G=1.f, const GLfloat B=1.f) {
  drawBall(p.x, p.y, radius, R, G, B);
}

inline void drawCircle(const GLfloat x, const GLfloat y, const GLfloat radius, const GLfloat R=1.f, const GLfloat G=1.f, const GLfloat B=1.f) {
  glBegin(GL_LINE_LOOP);
  glColor3f(R, G, B);
  int numSegments = 100;
  GLfloat angle;
  for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
    angle = i * 2.0f * PI / numSegments;  // 360 deg for all segments
    glVertex2f(x + cosf(angle) * radius, y + sinf(angle) * radius);
  }
  glEnd();
}

inline void drawCircle(const vec2 p, const GLfloat radius, const GLfloat R=1.f, const GLfloat G=1.f, const GLfloat B=1.f) {
  drawCircle(p.x, p.y, radius, R, G, B);
}

inline void drawTriangle( const GLfloat x, const GLfloat y, const GLfloat angle, 
                          const GLfloat scale, const GLfloat R=1.f, const GLfloat G=1.f, 
                          const GLfloat B=1.f) {
  const float pointScale = 3.f;
  glBegin(GL_TRIANGLE_FAN);
    glColor3f(R, G, B);
    glVertex2f(x + pointScale*scale*cosf(angle), y + pointScale*scale*sinf(angle));
    glVertex2f(x + scale*cosf(angle+1.5*PI), y + scale*sinf(angle+1.5*PI));
    glVertex2f(x + scale*cosf(angle-1.5*PI), y + scale*sinf(angle-1.5*PI));
  glEnd();
}

inline void drawTriangle(const vec2 p, const GLfloat angle, const GLfloat scale, const GLfloat R=1.f, const GLfloat G=1.f, const GLfloat B=1.f) {
  drawTriangle(p.x, p.y, angle, scale, R, G, B);
}
#endif // #if

#endif // __ANIMALS_OPENGL_OPENGL_UTILITY_HPP__
