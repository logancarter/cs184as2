#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>

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

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
  int w, h; // width and height
};

vector<string> split(const string &s, char delim, char delim2)
{
  vector<string> elems; 
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim) || getline(ss, item, delim2)) {
    cout << item << " thats item" << endl;
    if (item.compare(" ") != 0 && item.compare("\n") != 0 && !item.empty()) {
      cout << item << " thats item" << endl;
      elems.push_back(item);
    }
  }
  return elems;
}

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;

// enum string_code {
//   ka,
//   kd,
//   ks,
//   sp,
//   pl,
//   dl,
//   error,
// };

// TODO: Make a data structure of the lights, can be dynamically filled
// GLfloat ka_r, ka_g, ka_b, kd_r, kd_g, kd_b, ks_r, ks_g, ks_b, sp_v, pl_x, pl_y, pl_z, pl_r, pl_g, pl_b, dl_x, dl_y, dl_z, dl_r, dl_g, dl_b;
// int hasAmbient, hasDiffuse, hasSpecular, hasPLight, hasDLight, lightptr;
// std::vector<Light *> lights;
float sub_div_param;
bool adaptive = false;//if true: adaptive; if false: uniform

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
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);// clear the color buffer

  glMatrixMode(GL_MODELVIEW);        // indicate we are specifying camera transformations
  glLoadIdentity();        // make sure transformation is "zero'd"


  // Start drawing
  // circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);
  // circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) * 0.9 / 2);

  glFlush();
  glutSwapBuffers();// swap buffers (we earlier set double buffer)
}


//****************************************************
// function that handles keyboard callback
//****************************************************
void myKeyboard(unsigned char key, int x, int y) {
  // TODO: Why won't it print here?
  switch (key) {
  case 0x20:
    cout << "space: exit" << endl;
    exit(0);
    break;
  case 's':
   cout << "s: toggle flat/smooth" << endl;
   break;
  case 'w':
   cout << "w: toggle filled/wireframe" << endl;
   break;
  // TODO: check if its = or + (shift)
  case '=':
    cout << "+: zoom in" << endl;
    break;
  case '-':
    cout << "-: zoom out" << endl;
    break;
  default:
    cout << "unidentified " << key << endl;
    break;

  }

}


void specialKeys(int key, int x, int y) {
  int mod = glutGetModifiers();
  if (mod == GLUT_ACTIVE_SHIFT) {
    switch (key) {
    case GLUT_KEY_UP:
      cout << "translate: up" << endl;
      break;
    case GLUT_KEY_DOWN:
      cout << "translate: down" << endl;
      break;
    case GLUT_KEY_RIGHT:
      cout << "translate: right" << endl;
      break;
    case GLUT_KEY_LEFT:
      cout << "translate: left" << endl;
      break;
    default:
      cout << "shift special " << key << endl;
      break;
    }
  } else {
    switch (key) {
    case GLUT_KEY_UP:
      cout << "rotate: up" << endl;
      break;
    case GLUT_KEY_DOWN:
      cout << "rotate: down" << endl;
      break;
    case GLUT_KEY_RIGHT:
      cout << "rotate: right" << endl;
      break;
    case GLUT_KEY_LEFT:
      cout << "rotate: left" << endl;
      break;
    default:
      cout << "special " << key << endl;
      break;
    }
  }
}


//****************************************************
// function that hashes arguments, switch-able
//****************************************************
// string_code hashstring (std::string const& inString) {
//   if (inString == "-ka") return ka;
//   if (inString == "-kd") return kd;
//   if (inString == "-ks") return ks;
//   if (inString == "-sp") return sp;
//   if (inString == "-pl") return pl;
//   if (inString == "-dl") return dl;
//   return error;
// }

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

  //*******************************
  // ARGUMENT PARSER
  //*******************************
  string STRING;
  ifstream infile;
  cout << argv[1] << endl;//that's input file
  infile.open (argv[1]);
  int numpatches = 0;
  if (!infile.eof()) {
    getline(infile,STRING);
    numpatches = atoi(STRING.c_str());
  }
  while(!infile.eof()) { // To get you all the lines. Each iteration is one patch
    getline(infile,STRING);
    float a, b, c, d, e, f, g, h, i, j, k, l;
    std::istringstream iss (STRING);
    iss >> std::skipws >> a >> b >> c;
    iss >> std::skipws >> d >> e >> f;
    iss >> std::skipws >> g >> h >> i;
    iss >> std::skipws >> j >> k >> l;
    cout << j << " THATS j " << k << " k " << l << " and l" << endl;
    //now a-c is one vertex, d-f is another, etc to make up one rectangle/patch
    //do something with them
    //should probably use vector
  }

  string strsubp = argv[2];
  sub_div_param = atof(strsubp.c_str());
  cout << sub_div_param << endl;
  if (argc > 3) {
    string nextarg;
    nextarg = argv[3];
    if (nextarg.compare("-a") == 0) {
      adaptive = true;
      cout << "adaptive" << endl;
    }
  } else {
    cout << "not adaptive" << endl;
  }

  infile.close();
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

  initScene();// quick function to set up scene

  glutDisplayFunc(myDisplay);// function to run when its time to draw something
  glutReshapeFunc(myReshape);// function to run when the window gets resized
  // glutIdleFunc(myDisplay);
  glutKeyboardFunc(myKeyboard);           // function to run when spacebar is pressed: should exit
  glutSpecialFunc(specialKeys);

  glutMainLoop();// infinite loop that will keep drawing and resizing

  return 0;
}

