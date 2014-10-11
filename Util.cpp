/* Util.cpp */

#include "Util.h"
#include <Eigen/Dense>

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

/*#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif*/

#include <time.h>
#include <math.h>
#include "CImg.h"

#define PI 3.14159265  // Should be used from mathlib

using namespace std;
using namespace Eigen;
using namespace cimg_library; 


//****************************************************
// VECTORZ
//****************************************************

Vectorz::Vectorz(float r, float g, float b) {
 	x = r; 
 	y = g; 
 	z = b;
}

void Vectorz::setValues(float x1, float y1, float z1) {
        x = x1; y = y1; z = z1;
      }

Vectorz Vectorz::flip() {
        Vectorz res;
        res.setX(-this->getX());
        res.setY(-this->getY());
        res.setZ(-this->getZ());
        return res;
      }

Vectorz Vectorz::normalize() {
        Vectorz res;
        res.setValues(this->getX() / this->length(), this->getY() / this->length(), this->getZ() / this->length());
        return res;
      }

void Vectorz::printV() {
	cout << this->getX() << " " << this->getY() << " " << this->getZ() << endl;
}


//****************************************************
// MATRIX
//****************************************************

Matrixz::Matrixz() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			mat[i][j] = 0;
		}
	}

}


//****************************************************
// POINT
//****************************************************

void Point::setValues(float x1, float y1, float z1) {
	this->x = x1;
	this->y = y1;
	this->z = z1;
}

//****************************************************
// RAY
//****************************************************

// class Ray {
// 	// Point pos;
// 	// Vectorz dir;
// 	float t_min, t_max;
// 	Vector4f pos;
// 	Vector4f dir;

// public:
// 	// ray(t) = pos + t * dir
// 	Ray();

// };





//****************************************************
// CAMERA
//****************************************************

// class Camera
// {
// 	float eye_x, eye_y, eye_z;
// 	int width, height;
// 	// TODO: mapping from for the input corners to output size

// public:
// 	Camera(); // four corners, width, height, eye
// 	float getX() { return eye_x; }
// 	float getY() { return eye_y; }
// 	float getZ() { return eye_z; }
// 	// Vector4f get_eye() { 
// 	float* get_eye() {
// 		static float arr[3] = {eye_x, eye_y, eye_z};
// 		return arr;
// 	}

// };


//****************************************************
// SAMPLE
//****************************************************

Sample::Sample(float xval, float yval, float default_color) {
	x = xval;
	y = yval;
	color = default_color;
}


//****************************************************
// SAMPLER (TODO: Discuss whether we need this extra wasted space, rather than working with CImg directly)
//****************************************************

Sampler::Sampler() {
	width = 1000;
	height = 500;
	curr_x = curr_y = done = 0;
}

Sampler::Sampler(int w, int h) {
	width = w;
	height = h;
	curr_x = curr_y = done = 0;
}

Sample Sampler::getNextSample() {
	curr_x++;
	if (curr_x >= width) {
		curr_x = 0;
		curr_y++;
		if (curr_y >= height) {
			done = 1;
		}
	}
	int color = 0;
	// Test...later add the function that will calculate color.
	// if (curr_x > 200 && curr_x < 300) {
	// 	color = 243;
	// }
	return *(new Sample(curr_x, curr_y, color));
}



//****************************************************
// FILM
//****************************************************

Film::Film() {
	image.assign(1000, 500, 1, 3, 0); 
}

Film::Film(int w, int h, int z, int v, int default_color) {
	image.assign(w, h, z, v, default_color); 
}

void Film::setPixel(int x, int y, int z, int v, int color) {
	image(x, y, z, v) = color;
}

void Film::displayToScreen() {
	image.display();
}


//****************************************************
// SCENE
//****************************************************

Scene::Scene(Sampler &s, Film& f) {
	sampler = s;
	film = f;

}
void Scene::render() {
	while (!sampler.isDone()) {
	    Sample sample = sampler.getNextSample();
	    for (int i = 0; i < 3; i++) {
	    	film.setPixel(sample.getX(), sample.getY(), 0, i, sample.getColor());
	    }
  	}
  	film.displayToScreen();
}


