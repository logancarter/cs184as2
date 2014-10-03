#ifndef Matrix_H
#define Matrix_H

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

class Matrix {
	GLfloat mat[4][4];

	public:
		Matrix();
		GLfloat** getMatrix();
		Matrix createRotation();
		Matrix createTranslation();
		Matrix createScaling();
		// Matrix invert();
		// Matrix SVD();
};

#endif