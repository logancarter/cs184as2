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
	//cout << this->getX() << " " << this->getY() << " " << this->getZ() << endl;
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
// COLOR
//****************************************************


Color::Color() {
	r = g = b = 0.0;
}

Color::Color(float rv, float gv, float bv) {
	r = rv; g = gv; b = bv;
}
float Color::getR() { return r; }
float Color::getG() { return g; }
float Color::getB() { return b; }
void Color::setRGB(float rv, float gv, float bv) { r = rv; g = gv; b = bv; }



//****************************************************
// RAY
//****************************************************

Ray::Ray() {
	t_min = 0.0;
	t_max = 100000.0;
}

Ray::Ray(Vector4f eye, Vector4f pixel) {
	pos = eye;
	// TODO FIX NORMAL
	dir = pixel;
	dir.normalize();
	t_min = 0.0;
	t_max = 1000000.0;
}

void Ray::setEye(Vector4f eye) {
	pos = eye;
}

void Ray::setDir(Vector4f pminuse) {
	Vector3f temp;
	temp << pminuse(0), pminuse(1), pminuse(2);
	temp.normalize();
	dir << temp(0), temp(1), temp(2), 0;
	// dir.normalize();
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
  	// x = y = z = 0.0;
  	pos << 0.0, 0.0, 0.0, 1.0;
  	normal << 0.0, 0.0, 0.0, 0.0;
  }

  // LocalGeo::LocalGeo(float x1, float y1, float z1, float nx, float ny, float nz) {
  // 	pos(0) = x = x1;
  // 	pos(1) = y = y1;
  // 	pos(2) = z = z1;
  // 	pos(3) = 1;
  // 	normal(0) = nx;
  // 	normal(1) = ny;
  // 	normal(2) = nz;
  // 	normal(3) = 0;
  // 	normal.normalize();
  // }




//****************************************************
// INTERSECTION
//****************************************************

Intersection::Intersection() {
	lg = *(new LocalGeo());
	primitive = new Primitive();
}


Intersection::Intersection(LocalGeo local, Primitive &s) {
	lg = local;
	primitive = &s;
}


//****************************************************
// LIGHT
//****************************************************

void PointLight::getLightRay(Ray* light_ray, Color* light_color, LocalGeo lg) {
	light_ray->setEye(lg.getPos());
	light_ray->setDir(getPos() - lg.getPos());
	// cout << "light_ray" << endl << light_ray->getDir() << endl;
	light_color->setRGB(getRColor(), getGColor(), getBColor());
}

// TODO: fix this
void DirectionalLight::getLightRay(Ray* light_ray, Color* light_color, LocalGeo lg) {
	light_ray->setEye(lg.getPos());
	light_ray->setDir(getPos());
	light_ray->flipDir();
	light_color->setRGB(getRColor(), getGColor(), getBColor());
}

//****************************************************
// Primitive
//****************************************************

Primitive::Primitive() {

}

bool Primitive::intersect(Ray &ray, float *thit, Intersection* in) {
	//cout << "primitive intersect" << endl;
	/* Transform the ray w2o
	LocalGEo */
	return false;
}


bool Primitive::intersectP(Ray &lray) {
	return false;
}

// float Primitive::posMin(float t0, float t1) {
// 	if (std::abs(t0) > std::abs(t1)) {
// 		return t1;
// 	} else {
// 		return t0;
// 	}
// }


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

bool Sphere::testIntersect(float &a, float &b, float &c, float &x0, float &x1) {
	float d = (b * b) - (4.0 * a * c);
	if (d < 0) {
		//cout << "d < 0  " << d << endl;
		return false;
	} else if (d == 0) {
		x0 = x1 = -0.5 * b/a;
	} else {
		float q = (b > 0) ? 
		-0.5 * (b + sqrt(d)) :
		-0.5 * (b - sqrt(d));
		x0 = q/a;
		x1 = c/q;
	}
	// TODO-Check: hackde up some fixes
	// if (x0 != x0 || x1 != x1) {
	// 	//cout << "nan " << x0 << " " << x1 << endl;
	// 	// cout << "nans in da hosue" << endl;
	// 	// cout << posMin(x0,x1) << endl;
	// 	return false;
	// }
	// if (posMin(x0,x1) < 0.0) {	// TODO: make this slightly bigger than 0?
	// 	//cout << "neg " << x0 << " " << x1 << endl;
	// 	// cout << "negative's in da house!" << endl;
	// 	// cout << posMin(x0,x1) << endl;
	// 	return false;
	// }
	if (x0 > x1) {
		std::swap(x0, x1);
	}
	return true;
}


//algorithm credit goes to scratchapixel.com
bool Sphere::intersect(Ray &ray, float *thit, Intersection* in) {
	float t0 = 0.0;
	float t1 = 0.0;
	// cout << "sphere intersect" << endl;
	Vector4f difference = ray.getPos() - getCenter();
	float a = ray.getDir().dot(ray.getDir());
	float b = 2.0 * (ray.getDir()).dot(difference);
	float c = difference.dot(difference) - (getRadius() * getRadius());
	//cout << getRadius() << " is Radius\n";
	if (!testIntersect(a, b, c, t0, t1)) {
		return false;
	}

	if (t0 > ray.getTmax()) {
		return false;
	} else {
		ray.setTmax(t0);
		in->setPrimitive(this);
	}
	if (t0 < 0.0) {
		return false;
	}

	// if (posMin(t0, t1) < ray.getTmin()) {
	// 	//return false;
	// } else {
	// 	ray.setTmin(t1);
	// }
	*thit = posMin(t0, t1);
	*thit = t0;
	//cout << *thit << endl;
	// TODO: do this only if its the cloest
	float t_hit = *thit;
	//cout << t_hit << "THIT" << endl;
	//in->setPrimitive(this);
	Vector4f intersectionPoint;
	intersectionPoint = ray.getPos() + t_hit * ray.getDir();
	LocalGeo lg = *(new LocalGeo());
	lg.setPos(intersectionPoint);
	// cout << getCenter() << endl;
	// TODO: change the normal
	Vector4f normal = intersectionPoint - getCenter();
	normal.normalize();
	lg.setNormal(normal);
	in->setLocalGeo(lg);
	return true;
}

bool Sphere::intersectP(Ray &lray) {
	float t0 = 0;
	float t1 = 0;
	// cout << "sphere intersect" << endl;
	Vector4f difference = lray.getPos() - getCenter();
	float a = lray.getDir().dot(lray.getDir());
	float b = 2 * (lray.getDir()).dot(difference);
	float c = difference.dot(difference) - (getRadius() * getRadius());
	if (!testIntersect(a, b, c, t0, t1)) {
		return false;
	}
	if (t0 > lray.getTmax()) {
		return false;
	} else {
		lray.setTmax(t0);
	}
	if (t0 < 0.0) {
		return false;
	}
	return true;
}

//****************************************************
// TRIANGLE       
//****************************************************

  Triangle::Triangle() {

  }

  Triangle::Triangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz) {
  	a(0) = ax; a(1) = ay; a(2) = az;// a(3) = 1;
  	b(0) = bx; b(1) = by; b(2) = bz; //b(3) = 1;
  	c(0) = cx; c(1) = cy; c(2) = cz; //b(3) = 1;
  }

  bool Triangle::intersect(Ray &ray, float *thit, Intersection* in) {
  	Vector3f rayposition;
  	Vector3f raydir;
  	rayposition(0) = ray.getPos()[0]; rayposition(1) = ray.getPos()[1]; rayposition(2) = ray.getPos()[2];
  	raydir(0) = ray.getDir()[0]; raydir(1) = ray.getDir()[1]; raydir(2) = ray.getDir()[2];
  	Vector3f A;
  	Vector3f B;
  	A = b - a;
  	B = c - a;
  	Vector3f N = A.cross(B);
  	float NdotRayDir = N.dot(raydir);
  	if (NdotRayDir == 0) {
  		return false; //they're parallel
  	}
  	float d = N.dot(a);
  	float t = -(N.dot(rayposition) + d) / NdotRayDir;
  	if (t < 0) {
  		return false;
  	}
  	Vector3f P = rayposition + t * raydir;
  	Vector3f C;
  	Vector3f edge0 = b - a;
  	Vector3f VP0 = P - a;
  	C = edge0.cross(VP0);
  	if (N.dot(C) < 0) {
  		return false;
  	}

  	Vector3f edge1 = c - b;
  	Vector3f VP1 = P - b;
  	C = edge1.cross(VP1);
  	if (N.dot(C) < 0) {
  		return false;
  	}

  	Vector3f edge2 = a - c;
  	Vector3f VP2 = P - c;
  	C = edge2.cross(VP2);//is this right?
  	if (N.dot(C) < 0) {
  		return false;
  	}
  	if (t > ray.getTmax()) {
  		return false;
  	} else {
  		ray.setTmax(t);
  		in->setPrimitive(this);
  	}
  	*thit = t;
  	return true;
  }

  bool Triangle::intersectP(Ray &lray) {
  	float notUsed = 0; // unused garbage
  	Intersection* in = new Intersection();
  	return this->intersect(lray, &notUsed, in);
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


void Film::setPixel(int x, int y, Color& color) {
	float rgb[] = {color.getR(), color.getG(), color.getB()};
	// cout << rgb[0] << rgb[1] << rgb[2] << endl;
	// rgb[0] = 0.6;
	// rgb[1] = 0.6;
	// rgb[2] = 0.9;
	image.draw_point(x, y, rgb);
}

void Film::displayToScreen() {
	// image.mirror('y');
	// TODO: to flip or not to fliP?!??!?!?!??!!!??!?!??!?!?!?!???!
	image.mirror('x');
	image.normalize(0, 255);
	image.display();
	image.save("file.bmp");
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

	float u = x / width;
	float v = y / height;

	Vector4f pixel_vec = u * ((v * ll) + ((1 - v) * ul)) + (1 - u) * ((v * lr) + ((1 - v) * ur));
	// x = x * scale_w + ll[0];
	// y = y * scale_h + ll[1];
	// cout << x << " X " << y <<" y\n";
	//cout << pixel_vec(0) <<" pixelvecx " << pixel_vec(1) << " pixelvecy end\n";

	// Vector4f eye_vec(eye_x, eye_y, eye_z, 1);	// See if we can abstract this out to class var to avoid reconstructing everytime. (Done!)
	//Vector4f pixel_vec(x, y, z, 1);

	ray->setEye(eye);	// TODO: fix - redundant
	ray->setDir(pixel_vec - eye);
}





//****************************************************
// RAYTRACER
//****************************************************

RayTracer::RayTracer() {

}


RayTracer::RayTracer(std::vector<Primitive *> ps, std::vector<Light *> ls) {
	primitives = ps;
	lights = ls;
}

void RayTracer::addPrimitive(Primitive &primitive) {
	primitives.push_back(&primitive);
	//cout << primitives.size() << endl;
}

void RayTracer::addLight(Light &light) {
	lights.push_back(&light);
}

void RayTracer::trace(Ray& ray, int depth, Color* color) {
	color->setRGB(0.0, 0.0, 0.0);

	// TODO: Assume that object has coeffs, later handle if it doesn't.

	/***********************
	** FOR PRIMITIVES **
	***********************/
	for(std::vector<int>::size_type i = 0; i != primitives.size(); i++) {
		// cout << primitives.size() << endl;

		Primitive* primitive = primitives[i];
		// primitive->isPrimitive();
		float thit = 0.0;
		Intersection* in = new Intersection();

		/* DOES NOT INTERSECT */
		if (!primitive->intersect(ray, &thit, in)) {
			//cout << "THIT" << thit << endl;
			// TODO: Change this to look for ambient
			//color->setRGB(0.0, 0.0, 0.0);
			if (lights.empty()) {
				// sample->setBlack();
				//color->setRGB(0.0, 0.0, 0.0);
			}
		} 
		/* DOES INTERSECT */
		else {
			//cout << "thit" << thit << endl;
			// TODO: make this in->primitive (should give oyu the closest)
			BRDF* brdf = primitive->getMaterial()->getBRDF();
			Ray* light_ray = new Ray();
	        Color* light_color = new Color();

			/***********************
			** FOR LIGHTS **
			***********************/
			for(std::vector<int>::size_type k = 0; k != lights.size(); k++) {

	          	// light_color->setRGB(lights[k]->getRColor(), lights[k]->getGColor(), lights[k]->getBColor());
	          	bool isBlocked = false;
				lights[k]->getLightRay(light_ray, light_color, in->getLocalGeo());

				// TODO: Check if light is blocked, shadow ray
				for(std::vector<int>::size_type j = 0; j != primitives.size(); j++) {
					if (j == i) {
						continue;
					}
					isBlocked = primitives[j]->intersectP(*light_ray);
					if (isBlocked) {
						//cout << "is blocked" << endl;
						break;
					}
				}
				if (!isBlocked) {

			        if(lights[k]->isALight()) {
						// cout << k << " is ambient" << endl;
						Vector3f ambient(3);
						ambient(0) = lights[k]->getRColor();
						ambient(1) = lights[k]->getGColor();
						ambient(2) = lights[k]->getBColor();
						ambient = ambient.cwiseProduct(brdf->getKA());
						// RGB_result += ambient;
						color->appendRGB(ambient(0), ambient(1), ambient(2));
					} else {
			          Color temp = shade(in->getLocalGeo(), brdf, light_ray, light_color);
			          color->addColor(temp);
			      	}

		        }

	     	} 	// end For over lights

	      // color->setRGB(RGB_result(0), RGB_result(1), RGB_result(2));
	    }		// end else
        
    } //emd for over primitives
	    //TODO now do for each other primitive? or should I have already decided which primitive is the front?
	    // color->printV();
}


Color RayTracer::shade(LocalGeo lg, BRDF* brdf, Ray* light_ray, Color *light_color) {
	Color curr = *(new Color());
	Vector3f RGB_result;
	RGB_result(0) = RGB_result(1) = RGB_result(2) = 0.0;
    Vector3f pos, normal;
    Vector4f posh = lg.getPos();
    // Pos is the point on the shape.
    pos << posh(0), posh(1), posh(2);
    posh = lg.getNormal();
    normal << posh(0), posh(1), posh(2);

	Vector3f light, I_rgb;
	I_rgb << light_color->getR(), light_color->getG(), light_color->getB();
	light << light_ray->getDir()(0), light_ray->getDir()(1), light_ray->getDir()(2);
	light.normalize();

	Vector3f kd, diffuse;
	kd = brdf->getKD();
	diffuse = kd.cwiseProduct(I_rgb);
	diffuse = diffuse * fmax(light.dot(normal), 0.0);

	Vector3f ks, specular, reflection, viewer;
	ks = brdf->getKS();
	reflection = (light * -1) + (normal * light.dot(normal) * 2);
	reflection.normalize();
	// TODO: double check on this viewer direction...
	Vector3f ray_pos;
	ray_pos << getEye()(0), getEye()(1), getEye()(2);
	viewer = ray_pos - pos;
	viewer.normalize();
	specular = ks.cwiseProduct(I_rgb);
	specular = specular * pow(fmax(reflection.dot(viewer), 0.0), brdf->getKSP());

	Vector3f ka, ambient;
	ka = brdf->getKA();
	ambient = ka.cwiseProduct(I_rgb);

	Vector3f subtotal = diffuse + specular + ambient;
	RGB_result = RGB_result + subtotal;
 	curr.setRGB(RGB_result[0], RGB_result[1], RGB_result[2]);
    return curr;
  }


//****************************************************
// SCENE
//****************************************************

Scene::Scene(Sampler &s, Film& f, Camera &c, RayTracer &rt) {
	sampler = s;
	film = f;
	camera = c;
	raytracer = rt;
	raytracer.setEye(camera.get_eye());
}



void Scene::render() {
	Sample sample = *(new Sample());
	bool notDone = sampler.getNextSample(&sample);
	int depth = 0;
	while (notDone) {
		Ray ray = *(new Ray());
		camera.generateRay(sample, &ray);
		//Primitives[0]->isPrimitive();
		// raytracer.trace(ray, &sample, primitives, lights);
		raytracer.trace(ray, depth, &color);
		// if (sample.getX() == 107.0 && sample.getY() == 200.0) {
		if (sample.getX() == 279.0 && sample.getY() == 203.0) {
			cout << color.getR() << " " << color.getG() << " " << color.getB() << endl;
		}
    	// film.setPixel(sample.getX(), sample.getY(), 0, 0, 30000);
    	// film.setPixel(sample.getX(), sample.getY(), 0, 1, 0.0);
    	// film.setPixel(sample.getX(), sample.getY(), 0, 2, 0.0);
    	film.setPixel(sample.getX(), sample.getY(), color);
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
















