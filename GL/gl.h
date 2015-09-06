#ifndef _GL_GL_H_
#define _GL_GL_H_
// NOTE: page references are PDF pages in glspec.pdf, not the printed page numbers!

// standard types (p19, 2.4 Basic GL Operation)
typedef void GLvoid;
typedef uint8_t GLboolean;
typedef int8_t GLbyte;
typedef uint8_t GLubyte;
typedef int16_t GLshort;
typedef uint16_t GLushort;
typedef int32_t GLint;
typedef uint32_t GLuint;
typedef int32_t GLenum;
typedef size_t GLsizei;
typedef uint32_t GLbitfield;
// these use fixeds simply because they are MUCH faster
// accuracy issues? pfft, who cares about those?
typedef fixed GLfloat;
typedef fixed GLclampf;
typedef fixed64 GLdouble;
typedef fixed64 GLclampd;

#define GLtrue  1
#define GLfalse 0

// enum: 'E' errors (p22, 2.5 GL Errors, table 2.3)
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x4501
#define GL_INVALID_VALUE 0x4502
#define GL_INVALID_OPERATION 0x4503
#define GL_STACK_OVERFLOW 0x4504
#define GL_STACK_UNDERFLOW 0x4505
#define GL_OUT_OF_MEMORY 0x4506

// enum: 'M' matrices (p34, 2.9 Coordinate Transformations)
// we are probably going to ignore GL_PROJECTION.
#define GL_MODELVIEW 0x4D01
#define GL_TEXTURE 0x4D02
#define GL_PROJECTION 0x4D03

// enum: 'P' primitives (p25-27, 2.6 Begin/End Paradigm)
// we're keeping GL_POLYGON separate from GL_TRIANGLE_FAN
// so we can subdiv it nicely later if we feel like adding that support
// even though the latter is perfectly implementation-correct for GL_POLYGON
// "Only convex polygons are guaranteed to be drawn correctly by the GL." p25
#define GL_POINTS 0x5010
#define GL_LINES 0x5020
#define GL_LINE_STRIP 0x5021
#define GL_LINE_LOOP 0x5023
#define GL_TRIANGLES 0x5030
#define GL_TRIANGLE_STRIP 0x5031
#define GL_TRIANGLE_FAN 0x5033
#define GL_QUADS 0x5040
#define GL_QUAD_STRIP 0x5041
#define GL_POLYGON 0x5070

// begin
GLvoid glBegin(GLenum mode); // p24 2.6.1
GLvoid glEnd(GLvoid); // p24 2.6.1

// clear
GLvoid glClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
GLvoid glClear(GLbitfield mask);

// error
GLenum glGetError(GLvoid); // p20 2.5

// matrix
GLvoid glLoadIdentity(GLvoid); // p35 2.9.2
GLvoid glMatrixMode(GLenum mode); // p34 2.9.2
GLvoid glRotatef(GLfloat theta, GLfloat x, GLfloat y, GLfloat z); // p35 2.9.2
GLvoid glTranslatef(GLfloat x, GLfloat y, GLfloat z); // p36 2.9.2
GLvoid glPushMatrix(GLvoid); // p37 2.9.2
GLvoid glPopMatrix(GLvoid); // p37 2.9.2

// viewport
GLvoid glViewport(GLint x, GLint y, GLsizei w, GLsizei h); // p34 2.9.1

#endif
