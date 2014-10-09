#include <iostream>
#include <Eigen/Dense>
#include "Util.h"
#include <fstream>
#include <string>

vector<string> split(const string &s, char delim);

using namespace std;
using Eigen::MatrixXd;

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
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);       // clear the color buffer

  glMatrixMode(GL_MODELVIEW);             // indicate we are specifying camera transformations
  glLoadIdentity();               // make sure transformation is "zero'd"


  // Start drawing
  // circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);
  // circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) * 0.9 / 2);

  glFlush();
  glutSwapBuffers();          // swap buffers (we earlier set double buffer)
}

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

  //*******************************
  // ARGUMENT PARSER
  //*******************************

  // hasAmbient = hasDiffuse = hasSpecular = hasPLight = hasDLight = lightptr = 0;

  // for (int i = 1; i < argc; i++) {
  //   // cout<<"iterate: " <<argv[i]<<"\n";
  //   string curr = argv[i];
  //   // TODO: Can we assume that the arguments will be called correctly?
  //   switch(hashstring(curr)) {
  //     case ka: 
  //       // cout << "ambient" << endl;
  //       hasAmbient = 1;
  //       ka_r = (GLfloat) atof(argv[i+1]);
  //       ka_g = (GLfloat) atof(argv[i+2]);
  //       ka_b = (GLfloat) atof(argv[i+3]);
  //       // cout << ka_r << ka_g << ka_b << endl;
  //       i += 3;
  //       break;
  //     case kd: 
  //       // cout << "diffuse" << endl; 
  //       hasDiffuse = 1;
  //       kd_r = (GLfloat) atof(argv[i+1]);
  //       kd_g = (GLfloat) atof(argv[i+2]);
  //       kd_b = (GLfloat) atof(argv[i+3]);
  //       i += 3;
  //       // cout << kd_r << kd_g << kd_b << endl;
  //       break;
  //     case ks: 
  //       // cout << "specular" << endl; 
  //       hasSpecular = 1;
  //       ks_r = (GLfloat) atof(argv[i+1]);
  //       ks_g = (GLfloat) atof(argv[i+2]);
  //       ks_b = (GLfloat) atof(argv[i+3]);
  //       i += 3;
  //       // cout << ks_r << ks_g << ks_b << endl;
  //       break;
  //     case sp: 
  //       // cout << "power coefficient" << endl; 
  //       sp_v = (GLfloat) atof(argv[i+1]);
  //       i += 1;
  //       // cout << sp_v << endl;
  //       break;
  //     case pl: {
  //       cout << "point light" << endl;
  //       hasPLight = 1;
  //       pl_x = (GLfloat) atof(argv[i+1]);
  //       pl_y = (GLfloat) atof(argv[i+2]);
  //       pl_z = (GLfloat) atof(argv[i+3]);
  //       pl_r = (GLfloat) atof(argv[i+4]);
  //       pl_g = (GLfloat) atof(argv[i+5]);
  //       pl_b = (GLfloat) atof(argv[i+6]);
  //       i += 6;
  //       PointLight *plight = new PointLight();
  //       (*plight).setValues(pl_x,pl_y,pl_z,pl_r,pl_g,pl_b);
  //       lights.push_back(plight);

  //       lightptr++;
  //       // cout << pl_x << " " << pl_y << " " << pl_z << " " << pl_r << " " << pl_g << " " << pl_b << endl;
  //       } break;
  //     case dl: {
  //       // cout << "directional light" << endl; 

  //       hasDLight = 1;
  //       dl_x = (GLfloat) atof(argv[i+1]);
  //       dl_y = (GLfloat) atof(argv[i+2]);
  //       dl_z = (GLfloat) atof(argv[i+3]);
  //       dl_r = (GLfloat) atof(argv[i+4]);
  //       dl_g = (GLfloat) atof(argv[i+5]);
  //       dl_b = (GLfloat) atof(argv[i+6]);
  //       i += 6;
  //       DirectionalLight *dlight = new DirectionalLight();
  //       (*dlight).setValues(dl_x,dl_y,dl_z,dl_r,dl_g,dl_b);
  //       lights.push_back(dlight);
  //       lightptr++;
  //       // cout << dl_x << " " << dl_y << " " << dl_z << " " << dl_r << " " << dl_g << " " << dl_b << endl;
  //       } break;
  //     default: cout << "Invalid argument " << argv[i] << endl;
  //   } 
  // }

  // cout << hasDLight << hasPLight << endl;


  // for (int k = 0; k < lights.size(); k++) {
  //   Vectorz lightpos;
  //   lightpos.setValues(lights[k]->x, lights[k]->y, lights[k]->z);
  //   Vectorz I_rgb;
  //   I_rgb.setValues(lights[k]->r, lights[k]->g, lights[k]->b);
  //   lightpos.printV();
  //   I_rgb.printV();
  //   // cout << lights[k] << endl;
  // }

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();              // quick function to set up scene

  glutDisplayFunc(myDisplay);       // function to run when its time to draw something
  glutReshapeFunc(myReshape);       // function to run when the window gets resized
  glutKeyboardFunc(myKeyboard);           // function to run when spacebar is pressed: should exit

  glutMainLoop();             // infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}