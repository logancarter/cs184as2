#include <iostream>
#include <Eigen/Dense>
#include "Util.h"
#include <fstream>
#include <string>
#include "CImg.h"

vector<string> split(const string &s, char delim);

using namespace std;
using Eigen::MatrixXd;
using namespace cimg_library; 

// int main()
// {
// 	cout << "hello world!" << endl;
// 	Vectorz v1 = Vectorz(0.0, 1.0, 6.7);
// 	v1.printV();
//   MatrixXd m(2,2);
//   m(0,0) = 3;
//   m(1,0) = 2.5;
//   m(0,1) = -1;
//   m(1,1) = m(1,0) + m(0,1);
//   std::cout << m << std::endl;
//   return 0; 
// }


//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};

//****************************************************
// Global Variables
//****************************************************
Viewport  viewport;
// enum string_code {
//     ka,
//     kd,
//     ks,
//     sp,
//     pl,
//     dl,
//     error,
// };

// // TODO: Make a data structure of the lights, can be dynamically filled
//   GLfloat ka_r, ka_g, ka_b, kd_r, kd_g, kd_b, ks_r, ks_g, ks_b, sp_v, pl_x, pl_y, pl_z, pl_r, pl_g, pl_b, dl_x, dl_y, dl_z, dl_r, dl_g, dl_b;
//   int hasAmbient, hasDiffuse, hasSpecular, hasPLight, hasDLight, lightptr;
  // std::vector<Light *> lights;


//****************************************************
// function that handles keyboard callback
//****************************************************
void myKeyboard(unsigned char key, int x, int y) {
  // TODO: Why won't it print here?
  switch (key) {
    case 0x20:
      exit(0);
      break;
    } 
}

vector<string> split(const string &s, char delim, char delim2)
{
  vector<string> elems; 

  stringstream ss(s);
  string item;
  while (getline(ss, item, delim) || getline(ss, item, delim2)) {
    elems.push_back(item);
  }
  return elems;
}

int main(int argc, char *argv[]) {

  //*******************************
  // ARGUMENT PARSER
  //*******************************
    string STRING;
    ifstream infile;
    infile.open (argv[1]);
          while(!infile.eof()) { // To get you all the lines.
            getline(infile,STRING); // Current line saved in String
            vector<string> words = split(STRING, ' ', '\t');
            string currentword = words.at(0);
            if (currentword.compare("cam") == 0) {
              float ex = atof(words.at(1).c_str());
              float ey = atof(words.at(2).c_str());
              float ez = atof(words.at(3).c_str());
              float llx = atof(words.at(4).c_str());
              float lly = atof(words.at(5).c_str());
              float llz = atof(words.at(6).c_str());
              float lrx = atof(words.at(7).c_str());
              float lry = atof(words.at(8).c_str());
              float lrz = atof(words.at(9).c_str());
              float ulx = atof(words.at(10).c_str());
              float uly = atof(words.at(11).c_str());
              float ulz = atof(words.at(12).c_str());
              float urx = atof(words.at(13).c_str());
              float ury = atof(words.at(14).c_str());
              float urz = atof(words.at(15).c_str());
              if (words.size() > 16) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("sph") == 0) {
              float cx = atof(words.at(1).c_str());
              float cy = atof(words.at(2).c_str());
              float cz = atof(words.at(3).c_str());
              float r = atof(words.at(4).c_str());
              if (words.size() > 5) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("tri") == 0) {
              float ax = atof(words.at(1).c_str());
              float ay = atof(words.at(2).c_str());
              float az = atof(words.at(3).c_str());
              float bx = atof(words.at(4).c_str());
              float by = atof(words.at(5).c_str());
              float bz = atof(words.at(6).c_str());
              float cx = atof(words.at(7).c_str());
              float cy = atof(words.at(8).c_str());
              float cz = atof(words.at(9).c_str());
              if (words.size() > 10) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("obj") == 0) {
              ifstream newfile;
              newfile.open(words.at(1));
              newfile.close();
              if (words.size() > 2) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("ltp") == 0) {
              float px = atof(words.at(1).c_str());
              float py = atof(words.at(2).c_str());
              float pz = atof(words.at(3).c_str());
              float r = atof(words.at(4).c_str());
              float g = atof(words.at(5).c_str());
              float b = atof(words.at(6).c_str());
              float falloff = 0;
              if (words.size() > 7) {
                falloff = atof(words.at(7).c_str());
              }
              if (words.size() > 8) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("ltd") == 0) {
              float dx = atof(words.at(1).c_str());
              float dy = atof(words.at(2).c_str());
              float dz = atof(words.at(3).c_str());
              float r = atof(words.at(4).c_str());
              float g = atof(words.at(5).c_str());
              float b = atof(words.at(6).c_str());
              if (words.size() > 7) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("lta") == 0) {
              float r = atof(words.at(1).c_str());
              float g = atof(words.at(2).c_str());
              float b = atof(words.at(3).c_str());
              if (words.size() > 4) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("mat") == 0) {
              float kar = atof(words.at(1).c_str());
              float kag = atof(words.at(2).c_str());
              float kab = atof(words.at(3).c_str());
              float kdr = atof(words.at(4).c_str());
              float kdg = atof(words.at(5).c_str());
              float kdb = atof(words.at(6).c_str());
              float ksr = atof(words.at(7).c_str());
              float ksg = atof(words.at(8).c_str());
              float ksb = atof(words.at(9).c_str());
              float ksp = atof(words.at(10).c_str());
              float krr = atof(words.at(11).c_str());
              float krg = atof(words.at(12).c_str());
              float krb = atof(words.at(13).c_str());
              if (words.size() > 14) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("xft") == 0) {
              float tx = atof(words.at(1).c_str());
              float ty = atof(words.at(2).c_str());
              float tz = atof(words.at(3).c_str());
              if (words.size() > 4) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("xfr") == 0) {
              float rx = atof(words.at(1).c_str());
              float ry = atof(words.at(2).c_str());
              float rz = atof(words.at(3).c_str());
              if (words.size() > 4) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("xfs") == 0) {
              float sx = atof(words.at(1).c_str());
              float sy = atof(words.at(2).c_str());
              float sz = atof(words.at(3).c_str());
              if (words.size() > 4) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else if (currentword.compare("xfz") == 0) {
              //Need to reset the current transformation to identity
              if (words.size() > 1) {
                fprintf(stderr, "Warning: Extra arguments ignored.\n");
              }
            }
            else {
              fprintf(stderr, "Warning: Unsupported feature ignored.\n");
            }
         }
    infile.close();


  // TODO: Make this inputtable from filled
  // DEV: Hardcode this if you want to override inputs
  int width = 100;
  int height = 100;

  Sphere sphere = *(new Sphere(1, 0, 0, -2));

  Sampler sampler = *(new Sampler(width, height));
  Film film = *(new Film(width, height, 1, 3, 0));
  Camera camera = *(new Camera(0,0,0,width,height,-1,-1,-1,1,-1,-1,-1,1,-1,1,1,-1));
  RayTracer raytracer = *(new RayTracer());
  Scene scene = *(new Scene(sampler, film, camera, raytracer));
  scene.render();

  return 0;
}