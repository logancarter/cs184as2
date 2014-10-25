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
    coordinates(0) = x1;
    coordinates(1) = y1;
    coordinates(2) = z1;
    coordinates(3) = 1;
}

//****************************************************
// RAY
//****************************************************

Ray::Ray() {

}

Ray::Ray(Vector4f eye, Vector4f pixel) {
	pos = eye;
	dir = pixel - eye;
}

void Ray::setEye(Vector4f eye) {
	pos = eye;
}

void Ray::setDir(Vector4f pixel) {
	dir = pixel - pos;
}

Vector4f Ray::getDir() {
	return dir;
}

Vector4f Ray::getPos() {
	return pos;
}


//****************************************************
// LCAOLGEO
//****************************************************

  LocalGeo::LocalGeo() {

  }

  LocalGeo::LocalGeo(float x1, float y1, float z1, float nx, float ny, float nz) {
  	pos(0) = x = x1;
  	pos(1) = y = y1;
  	pos(2) = z = z1;
  	pos(3) = 1;
  	normal(0) = nx;
  	normal(1) = ny;
  	normal(2) = nz;
  	normal(3) = 0;
  	normal.normalize();
  }

//****************************************************
// INTERSECTION
//****************************************************

Intersection::Intersection() {

}


Intersection::Intersection(LocalGeo local, Primitive &s) {
	lg = local;
	primitive = &s;
}


//****************************************************
// Primitive
//****************************************************

Primitive::Primitive() {

}

bool Primitive::intersect(Ray& ray, float *thit, Intersection* in) {
	cout << "primitive intersect" << endl;
	/* Transform the ray w2o
	LocalGEo */
	return false;
}

//****************************************************
// SPHERE
//****************************************************

Sphere::Sphere() {

}

Sphere::Sphere(float r, float x, float y, float z) {
	radius = r;
	center_x = x;
	center_y = y;
	center_z = z;
}

Vector4f Sphere::getCenter() {
	Vector4f v(center_x, center_y, center_z, 1);
	return v;
}

float Sphere::getRadius() {
	return radius;
}

bool Sphere::testIntersect(float a, float b, float c, float &x0, float &x1) {
	//cout << "test Intersect" << endl;
	float d = (b*b) - (4 * a * c);
	if (d < 0) {
		//cout << "\ntestIntersect  " << d;
		return false;
	}
	else if (d == 0) {
		x0 = x1 = -0.5 * b/a;
	}
	else {
		float q = (b > 0) ? 
		-0.5 * (b + sqrt(d)) :
		-0.5 * (b - sqrt(d));
		x0 = q/a;
		x1 = c/q;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
	}
	return true;
}

//algorithm credit goes to scratchapixel.com
bool Sphere::intersect(Ray& ray, float *thit, Intersection* in) {
	float t0;
	float t1;
	cout << "sphere intersect" << endl;
	Vector4f difference = ray.getPos() - getCenter();
	float a = ray.getDir().dot(ray.getDir());
	float b = 2 * (ray.getDir()).dot(difference);
	float c = difference.dot(difference) - getRadius();
	if (!testIntersect(a, b, c, t0, t1)) {
		return false;
	}
	// if (t0 > ray.tmax) {
	// 	return false;
	// } else {
	// 	ray.tmax = 0;
	// }
	return true;
}

//****************************************************
// TRIANGLE       
//****************************************************

  Triangle::Triangle() {

  }

  Triangle::Triangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz) {
  	a.setValues(ax, ay, az);
  	b.setValues(bx, by, bz);
  	c.setValues(cx, cy, cz);
  }





//****************************************************
// TRANSFORMATION
//****************************************************

Transformation::Transformation() {

}

void Transformation::setMat(Matrix4f m) {
	mat = m;
	inv = m.inverse();
}

Matrix4f Transformation::getMat() {
	return mat;
}

Matrix4f Transformation::getInv() {
	return inv;
}


//****************************************************
// COLOR
//****************************************************


Color::Color() {

}

Color::Color(float rv, float gv, float bv) {
	r = rv; g = gv; b = bv;
}
float Color::getR() { return r; }
float Color::getG() { return g; }
float Color::getB() { return b; }
void Color::setRGB(float rv, float gv, float bv) { r = rv; g = gv; b = bv; }


//**************************************************** 
// SAMPLE
//****************************************************

Sample::Sample() {
	x = 0;
	y = 0;
	r = 0;
	g = 0;
	b = 0;
	//color = null;
}

Sample::Sample(float xval, float yval, float r1, float g1, float b1) {
	x = xval;
	y = yval;
	r = r1;
	g = g1;
	b = b1;
}


//****************************************************
// SAMPLER
//****************************************************

Sampler::Sampler() {
	// Garbage initialization
	width = 1000;
	height = 500;
	curr_x = -1;
	curr_y = 0;
}

Sampler::Sampler(int w, int h) {
	width = w;
	height = h;
	curr_x = curr_y = 0;
}

void Sampler::getFirstSample(Sample *sample) {
	sample->setX(0);
	sample->setY(0);
	sample->setRColor(0.0);
	sample->setGColor(0.0);
	sample->setBColor(0.0);
}

bool Sampler::getNextSample(Sample *sample) {
	curr_x++;
	if (curr_x >= width) {
		curr_x = 0;
		curr_y++;
		if (curr_y >= height) {
			return false;
		}
	}
	sample->setX(curr_x);
	sample->setY(curr_y);
	// Test...later add the function that will calculate color.
	// if (curr_x > 200 && curr_x < 300) {
	// 	sample->setColor(243);
	// } else {
	// 	sample->setColor(0.0);
	// }
	return true;
}



//****************************************************
// FILM
//****************************************************

Film::Film() {
	image.assign(1000, 500, 1, 3, 0); 
}

Film::Film(int w, int h, int z, int v, float default_color) {
	image.assign(w, h, z, v, default_color); 
}

// TODO: CHANGE THIS SO WE CAN SET MULTIPLE COLORSSSSSSS NOT JSUT 1111111 FUCK
void Film::setPixel(int x, int y, int z, int v, float color) {
	image(x, y, z, v) = color;
}

void Film::displayToScreen() {
	image.mirror('y');
	image.mirror('x');
	image.display();
}


//****************************************************
// CAMERA
//****************************************************

Camera::Camera() {
	eye_x = eye_y = eye_z = 0.0;
	width = height = 0;
}

Camera::Camera(float x, float y, float z, int w, int h, float llx, float lly, float llz, float lrx, float lry, float lrz, float ulx, float uly, float ulz, float urx, float ury, float urz) {
	eye_x = x; eye_y = y; eye_z = z;
	width = w;
	height = h;
	eye(0) = x;
	eye(1) = y;
	eye(2) = z;
	eye(3) = 1;
	ll(0) = llx;
	ll(1) = lly;
	ll(2) = llz;
	ll(3) = 1;
	lr(0) = lrx;
	lr(1) = lry;
	lr(2) = lrz;
	lr(3) = 1;
	ul(0) = ulx;
	ul(1) = uly;
	ul(2) = ulz;
	ul(3) = 1;
	ur(0) = urx;
	ur(1) = ury;
	ur(2) = urz;
	ur(3) = 1;
	plane_width = lrx - llx;
	plane_height = uly - lly;
	scale_w = plane_width / width;
	scale_h = plane_height / height;
}

// TODO
void Camera::generateRay(Sample sample, Ray *ray) {
	float x = sample.getX();
	float y = sample.getY();

	//float z = ll(2);	// TODO: assume its on same plane...checkcheckcheck
	//x = x * scale_w + ll[0];
	//y = y * scale_h + ll[1];

	float u = x /width;
	float v = y /height;

	Vector4f pixel_vec = u * ((v * ll) + ((1 - v) * ul)) + (1 - u) * ((v * lr) + ((1 - v) * ur));
	// x = x * scale_w + ll[0];
	// y = y * scale_h + ll[1];
	// cout << x << " X " << y <<" y\n";
	//cout << pixel_vec(0) <<" pixelvecx " << pixel_vec(1) << " pixelvecy end\n";

	// Vector4f eye_vec(eye_x, eye_y, eye_z, 1);	// See if we can abstract this out to class var to avoid reconstructing everytime. (Done!)
	//Vector4f pixel_vec(x, y, z, 1);
	ray->setEye(eye);	// TODO: fix - redundant
	ray->setDir(pixel_vec);
}





//****************************************************
// RAYTRACER
//****************************************************

RayTracer::RayTracer() {

}

// void RayTracer::trace(Ray& ray, int depth, Color* color) {

// }

// TODO: Assume that object has coeffs, later handle if it doesn't.
void RayTracer::trace(Ray ray, Sample *sample, std::vector<Primitive *> primitives, std::vector<Light *> lights) {
	for(std::vector<int>::size_type i = 0; i != primitives.size(); i++) {
		Primitive* primitive = primitives[i];
		primitive->isPrimitive();
		float* thit = 0;
		Intersection* in = new Intersection();
		if (!primitive->intersect(ray, thit, in)) {
			// TODO: Change this to look for ambient
			sample->setBlack();
			if (lights.empty()) {
				sample->setBlack();
			}
		} else {
			cout << "intersect" << endl;
			Vector3f RGB_result;
			for(std::vector<int>::size_type j = 0; j != lights.size(); j++) {
				cout << "HI" << endl;
				if(lights[j]->isALight()) {
					cout << lights[0]->isALight() << " it knows its ambient \n";
					Vector3f ambient(3);
					ambient(0) = lights[j]->getRColor();
					ambient(1) = lights[j]->getGColor();
					ambient(2) = lights[j]->getBColor();
					RGB_result = RGB_result + ambient;
				}

				// TODO: change this to take into account material
				sample->setRColor(RGB_result(0));
				sample->setGColor(RGB_result(1));
				sample->setBColor(RGB_result(2));

				/*
				// This is the front-facing Z coordinate
		        float z = sqrt(radius*radius-dist*dist);
		        Vectorz pos, normal;
		        pos.setValues(x,y,z);
		        normal = Vectorz::add(pos, pos);  
		        normal = normal.normalize();        

		        Vectorz result;
		        result.setValues(0,0,0);

		        // For each light source...
		        for (int k = 0; k < lightptr; k++) {
		          GLfloat r, g, b, x, y, z;
		          Vectorz lightpos, light;
		          lightpos.setValues(lights[k]->x, lights[k]->y, lights[k]->z);

		          Vectorz I_rgb;
		          I_rgb.setValues(lights[k]->r, lights[k]->g, lights[k]->b);

		          if (i == 200 && j == 200) {
		            cout << k << lights.size() << endl;
		            I_rgb.printV();
		            lightpos.printV();
		          }

		          if (lights[k]->isDLight()) {
		            light = lightpos.flip().normalize();
		          } else {        // Is point light
		            light = Vectorz::add(Vectorz::subtract(lightpos, pos), pos);
		            light = light.normalize();
		          }

		          Vectorz kd, diffuse;
		          if (hasDiffuse) {
		            kd.setValues(kd_r, kd_g, kd_b);
		            diffuse = Vectorz::elementMult(kd, I_rgb);
		            diffuse = Vectorz::scale(diffuse, fmax(Vectorz::dot(light, normal), 0.0));
		          } else {
		            diffuse.setValues(0,0,0);
		          }

		          Vectorz ks, specular, reflection, viewer;
		          if (hasSpecular) {
		            ks.setValues(ks_r, ks_g, ks_b);
		            reflection = Vectorz::add(light.flip(), Vectorz::scale(Vectorz::scale(normal, Vectorz::dot(light, normal)), 2));
		            reflection = reflection.flip();
		            reflection = reflection.normalize();
		            viewer.setValues(0,0,-1);
		            specular = Vectorz::elementMult(ks, I_rgb);
		            specular = Vectorz::scale(specular, pow(fmax(Vectorz::dot(reflection, viewer), 0.0), sp_v));
		          } else {
		            specular.setValues(0,0,0);
		          }

		          Vectorz ka, ambient;
		          if (hasAmbient) {
		            ka.setValues(ka_r, ka_g, ka_b);
		            ambient = Vectorz::elementMult(ka, I_rgb);
		          } else {
		            ambient.setValues(0,0,0);
		          }
		          
		          Vectorz subtotal = Vectorz::add(Vectorz::add(diffuse, specular), ambient);
		          result = Vectorz::add(result, subtotal);
		      }
*/
		      }
	        // setPixel(i,j, result.getX(), result.getY(), result.getZ());
	    }
	}
}



//****************************************************
// SCENE
//****************************************************

Scene::Scene(Sampler &s, Film& f, Camera &c, RayTracer &rt) {
	sampler = s;
	film = f;
	camera = c;
	raytracer = rt;
}

void Scene::addPrimitive(Primitive &primitive) {
	primitives.push_back(&primitive);
}

void Scene::addLight(Light &light) {
	lights.push_back(&light);
}

void Scene::render() {
	Sample sample = *(new Sample());
	Ray ray;
	bool notDone = sampler.getNextSample(&sample);
	while (notDone) {
		camera.generateRay(sample, &ray);
		//Primitives[0]->isPrimitive();
		raytracer.trace(ray, &sample, primitives, lights); 	// TODO: should be all primitves
    	film.setPixel(sample.getX(), sample.getY(), 0, 0, sample.getRColor());
    	film.setPixel(sample.getX(), sample.getY(), 0, 1, sample.getGColor());
    	film.setPixel(sample.getX(), sample.getY(), 0, 2, sample.getBColor());
	    notDone = sampler.getNextSample(&sample);
  	}
  	film.displayToScreen();
}

// bool scene::intersect(Ray &r, double &closest_t, 
//   GeometryProperties &geom_prop, MaterialProperties &mat_prop);
// {
//   closest_t = std::numeric_limits<double>::infinity();
//   std::vector<Primitive> relevant_prims = 
//     acceleration_root.relevant_prims(r);
//   for(std::vector<Primitive> prim_it = relevant_prims.begin();
//     prim_it != relevant_prims.end(); ++prim_it)
//   {
//     Primitive cur_prim = *prim_it;
//     ...
//   }
// }
















