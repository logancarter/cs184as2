#ifndef Ray_H
#define Ray_H

#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>

class Ray {
	Point pos;
	Vectorz dir;
	GLfloat t_min, t_max;

public:
	// ray(t) = pos + t * dir

};

#endif