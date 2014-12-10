/* Util.h */

#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <limits>

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
#include <math.h>

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;
using namespace Eigen;

//*******************************************
// JOINT
//*******************************************

class Joint {
public:
	Vector3f point;
	GLfloat radius;
	Joint(Vector3f p) {
		//point = *(new Vector3f(0.0, 0.0, 0.0));
		point = p;
		radius = 1.0;
	}
	void changePoint(Vector3f p) {
		point = p;
	}
};

class Arm {
public:
	GLfloat radius, length;
	Vector3f startpoint, endpoint;
	Arm (Vector3f start, Vector3f end, float len) {
		radius = 1.0;
		length = len;
		startpoint = start;
		endpoint = end;
	}
	void changePoints(Vector3f start, Vector3f end) {
		startpoint = start;
		endpoint = end;
	}
};



