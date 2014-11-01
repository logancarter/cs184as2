#include <iostream>
#include <Eigen/Dense>
#include "Util.h"
#include <fstream>
#include <string>
#include "CImg.h"
#include <vector>

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
Camera camera;
float width = 1000.0;
float height = 1000.0;
std::vector<Primitive *> primitives;
int numshapes = 0;
std::vector<Light *> lights;
std::vector<Transformation *> transformations;
Transformation* currentTrans = new Transformation();
int numtrans;
int numlights = 0;
Material* currentMaterial = new Material();


//****************************************************
// function that handles keyboard callback
//****************************************************
void myKeyboard(unsigned char key, int x, int y) {
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

vector<string> split(const string &s, char delim)
{
  vector<string> elems; 
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

void handle_obj(vector<string> words, char* file) {
  ifstream newfile;
  newfile.open(file);
  string curr_line;
  std::vector<Vector4f *> vertices;
  std::vector<Vector4f *> normalvertices;
  while (!newfile.eof()) {
    getline(newfile,curr_line);
    if (curr_line.compare("") == 0 || curr_line.compare("\n") == 0) {
      continue;
    }
    vector<string> currentwords = split(curr_line, ' ', '\t');
    string currentword = currentwords.at(0);
    if (currentword.compare("v") == 0) {
      float xval = atof(currentwords.at(1).c_str());
      float yval = atof(currentwords.at(2).c_str());
      float zval = atof(currentwords.at(3).c_str());
      Vector4f *vertex = new Vector4f(xval, yval, zval, 1);
      vertices.push_back(vertex);
    } else if (currentword.compare("f") == 0) {
        if (currentwords.at(1).find("//") != std::string::npos) {
          string tosplit = currentwords.at(1);
          vector<string> currentpart = split(tosplit, '/');
          float vnum1 = atof(currentpart[0].c_str());
          float surfacenum1 = atof(currentpart[2].c_str());
          tosplit = currentwords.at(2);
          currentpart = split(tosplit, '/');
          float vnum2 = atof(currentpart[0].c_str());
          float surfacenum2 = atof(currentpart[2].c_str());
          tosplit = currentwords.at(3);
          currentpart = split(tosplit, '/');
          float vnum3 = atof(currentpart[0].c_str());
          float surfacenum3 = atof(currentpart[2].c_str());

          Vector4f vertexone = *vertices[vnum1 - 1];
          Vector4f vertextwo = *vertices[vnum2 - 1];
          Vector4f vertexthree = *vertices[vnum3 - 1];
          Vector4f normalone =  *normalvertices[surfacenum1 - 1];
          Vector4f normaltwo =  *normalvertices[surfacenum2 - 1];
          Vector4f normalthree =  *normalvertices[surfacenum3 - 1];
          LocalGeo *one = new LocalGeo();
          LocalGeo *two = new LocalGeo();
          LocalGeo *three = new LocalGeo();
          //one->set(vertexone)
        } else {
          int first = atoi(currentwords.at(1).c_str());
          int second = atoi(currentwords.at(2).c_str());
          int third = atoi(currentwords.at(3).c_str());
          Vector4f vertexone = *vertices[first - 1];
          Vector4f vertextwo = *vertices.at(second - 1);
          Vector4f vertexthree = *vertices.at(third - 1);
          Triangle *triangle = new Triangle(vertexone[0], vertexone[1], vertexone[2], vertextwo[0], vertextwo[1], vertextwo[2], vertexthree[0], vertexthree[1], vertexthree[2]);
          if (currentMaterial) {
            triangle->setMaterial(*currentMaterial);
          }
          numshapes++;
          primitives.push_back(triangle);
        }
    } else if (currentword.compare("vn") == 0) {
        float ival = atof(currentwords.at(1).c_str());
        float jval = atof(currentwords.at(2).c_str());
        float kval = atof(currentwords.at(3).c_str());
        Vector4f *normalvertex = new Vector4f(ival, jval, kval, 1);
        normalvertices.push_back(normalvertex);
    } else {
      fprintf(stderr, "Warning: Unsupported feature ignored.\n");
    }
  }
  newfile.close();
}

int main(int argc, char *argv[]) {


  //*******************************
  // ARGUMENT PARSER
  //*******************************
  char* file;
    string STRING;
    ifstream infile;
      infile.open (argv[1]);
            while(!infile.eof()) { // To get you all the lines.
              getline(infile,STRING); // Current line saved in String
              vector<string> words = split(STRING, ' ', '\t');
              if (STRING.compare("") == 0 || STRING.compare("\n") == 0 || words.size() == 0) {
                continue;
              }
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
                camera = *(new Camera(ex,ey,ez,width,height,llx,lly,llz,lrx,lry,lrz,ulx,uly,ulz,urx,ury,urz));
              }
              else if (currentword.compare("sph") == 0) {
                float cx = atof(words.at(1).c_str());
                float cy = atof(words.at(2).c_str());
                float cz = atof(words.at(3).c_str());
                float r = atof(words.at(4).c_str());
                if (words.size() > 5) {
                  fprintf(stderr, "Warning: Extra arguments ignored.\n");
                }
                // TODO: do material for other primitives too, triangle...
                Sphere *sphere = new Sphere(r, cx, cy, cz);
                if (currentMaterial) {
                  sphere->setMaterial(*currentMaterial);
                }
                char numshapesstr[10];
                char sphstr[20];
                strcpy(sphstr, "sphere");
                sphstr[6] = '\0';
                sprintf(numshapesstr, "%d", numshapes);
                strcat(sphstr, numshapesstr);
                string str(sphstr);
                sphere->setName(sphstr);
                sphere->setO2W(*currentTrans);
                numshapes++;
                // if (numtrans > 0) {
                //   //add transformations
                //   for (int i = 0; i < numtrans; i ++) {
                //     sphere->setO2w(*transformations[i]);
                //   }
                // }
                primitives.push_back(sphere);

            } else if (currentword.compare("tri") == 0) {
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
                Triangle *triangle = new Triangle(ax, ay, az, bx, by, bz, cx, cy, cz);
                if (currentMaterial) {
                  triangle->setMaterial(*currentMaterial);
                }
                char numshapesstr[10];
                char tristr[20];
                strcpy(tristr, "triangle");
                tristr[8] = '\0';
                sprintf(numshapesstr, "%d", numshapes);
                strcat(tristr, numshapesstr);
                string str(tristr);
                triangle->setName(str);
                triangle->setO2W(*currentTrans);
                numshapes++;
                primitives.push_back(triangle);
              }
              else if (currentword.compare("obj") == 0) {
                string filename = words.at(1);
                //filename.erase(0, 1);
                //int length = filename.size() / sizeof(char);
                //filename.pop_back();
                char *cstr = &filename[0];
                handle_obj(words, cstr);
                if (words.size() > 2) {
                  fprintf(stderr, "Warning: Extra arguments ignored.\n");
                }
                // TODO: transform obj files
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
                PointLight* plight = new PointLight();
                plight->setValues(px, py, pz, r, g, b);
                lights.push_back(plight);
                numlights++;
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
                DirectionalLight* dlight = new DirectionalLight();
                dlight->setValues(dx, dy, dz, r, g, b);
                lights.push_back(dlight);
                numlights++;
              }
              else if (currentword.compare("lta") == 0) {
                float r = atof(words.at(1).c_str());
                float g = atof(words.at(2).c_str());
                float b = atof(words.at(3).c_str());
                if (words.size() > 4) {
                  fprintf(stderr, "Warning: Extra arguments ignored.\n");
                }
                AmbientLight* alight = new AmbientLight(r, g, b);
                lights.push_back(alight);
                numlights++;
              }
              else if (currentword.compare("mat") == 0) {
                // TODO: Check if all of these properties are there, handle if not.
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
                Material mat = *(new Material());
                mat.getBRDF()->setAmbient(kar, kag, kab);
                mat.getBRDF()->setDiffuse(kdr, kdg, kdb);
                mat.getBRDF()->setSpecular(ksr, ksg, ksb, ksp);
                mat.getBRDF()->setReflection(krr, krg, krb);
                currentMaterial = &mat;
              }
              else if (currentword.compare("xft") == 0) {
                float tx = atof(words.at(1).c_str());
                float ty = atof(words.at(2).c_str());
                float tz = atof(words.at(3).c_str());
                if (words.size() > 4) {
                  fprintf(stderr, "Warning: Extra arguments ignored.\n");
                }
                Matrix4f translate;
                translate(0,0) = 1;
                translate(1,1) = 1;
                translate(2,2) = 1;
                translate(3,3) = 1;
                translate(0,3) = tx;
                translate(1,3) = ty;
                translate(2,3) = tz;
                currentTrans->setMat(currentTrans->getMat() * translate);
                // Transformation* trans = new Transformation();
                // trans->setMat(translate);
                // transformations.push_back(trans);
                numtrans++;
              }
              else if (currentword.compare("xfr") == 0) {
                float rx = atof(words.at(1).c_str());
                float ry = atof(words.at(2).c_str());
                float rz = atof(words.at(3).c_str());
                if (words.size() > 4) {
                  fprintf(stderr, "Warning: Extra arguments ignored.\n");
                }
                Vector3f r;
                r << rx, ry, rz;
                r.normalize();
                Matrix4f rotate;
                // rotate << 0, -r(2), r(1), 0,
                //           r(2), 0, -r(0), 0,
                //           -r(1), r(0), 0, 1;
                currentTrans->setMat(currentTrans->getMat() * rotate);
                // cout << "ROtate!" << endl;
              }
              else if (currentword.compare("xfs") == 0) {
                float sx = atof(words.at(1).c_str());
                float sy = atof(words.at(2).c_str());
                float sz = atof(words.at(3).c_str());
                if (words.size() > 4) {
                  fprintf(stderr, "Warning: Extra arguments ignored.\n");
                }
                Matrix4f scale;
                scale(0,0) = sx;
                scale(1,1) = sy;
                scale(2,2) = sz;
                scale(3,3) = 1;
                currentTrans->setMat(currentTrans->getMat() * scale);
                // Transformation* trans = new Transformation();
                // trans->setMat(scale);
                // transformations.push_back(trans);
                numtrans++;
              }
              else if (currentword.compare("xfz") == 0) {
                //Need to reset the current transformation to identity
                if (words.size() > 1) {
                  fprintf(stderr, "Warning: Extra arguments ignored.\n");
                }
                currentTrans->setID();
                transformations.clear();
              }
              else {
                fprintf(stderr, "Warning: Unsupported feature ignored.\n");
              }
           }
      infile.close();

  // TODO: Make this inputtable from filled
  // DEV: Hardcode this if you want to override inputs


  // Hardcode camera to be at origin, with the image place from (-1,-1,-1) to (1,1,-1)
  //Camera camera = *(new Camera(0,0,0,width,height,-1,-1,-1,1,-1,-1,-1,1,-1,1,1,-1));
  //Camera camera = *(new Camera(0,0,0,width,height,-1,-1,-0,1,-1,-2,-1,1,0,1,1,-2));
  //Camera camera = *(new Camera(0,0,0,width,height,-1,-1,2,1,-1,-1,-1,1,-1,1,1,20));

  //camera = *(new Camera(0,0,0,width,height,-1,-1,-0,1,-1,-2,-1,1,0,1,1,-2));
  Film film = *(new Film(width, height, 1, 3, 0));
  Sampler sampler = *(new Sampler(width, height));
  Vector4f rt_eye;
  rt_eye << camera.getX(), camera.getY(), camera.getZ(), 1;
  RayTracer raytracer = *(new RayTracer(rt_eye));
  //make a list of shapes and add them!
  //scene.addShape(sphere);
  for (int i = 0; i < numshapes; i++) {
    raytracer.addPrimitive(*primitives[i]);
  }

  for (int i = 0; i < numlights; i++) {
    raytracer.addLight(*lights[i]);
  }
  //cout << "size of ray's prims " << raytracer.getPrims().size() << endl;
  Scene scene = *(new Scene(sampler, film, camera, raytracer));
  scene.render();
  return 0;
}