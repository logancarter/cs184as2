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
bool adaptive = false;      //if true: adaptive; if false: uniform
std::vector<int *> patches;
std::vector<GLfloat> points;    // hack for now



//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // gluOrtho2D(0, viewport.w, 0, viewport.h);

  //----------- setting the projection -------------------------
  // glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


  // glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
  // glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

  glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch

  //------------------------------------------------------------

}

//****************************************************
// Simple init function
//****************************************************
void initScene(){

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
  myReshape(viewport.w,viewport.h);
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

  glClear(GL_COLOR_BUFFER_BIT);       // clear the color buffer

  glMatrixMode(GL_MODELVIEW);        // indicate we are specifying camera transformations
  glLoadIdentity();        // make sure transformation is "zero'd"


    //----------------------- code to draw objects --------------------------
  glColor3f(1.0f,0.0f,0.0f);                   // setting the color to pure red 90% for the rect


  // Left rectangle
  glBegin(GL_POLYGON);                         // draw rectangle 
  //glVertex3f(x val, y val, z val (won't change the point because of the projection type));
  glVertex3f(-1.0f, 0.0f, 0.0f);               // bottom left corner of rectangle
  glVertex3f(-1.0f, 0.5f, 0.0f);               // top left corner of rectangle
  glVertex3f(-0.95f, 0.5f, 0.0f);               // top right corner of rectangle
  glVertex3f(-0.95f, 0.0f, 0.0f);               // bottom right corner of rectangle
  glEnd();

  glBegin(GL_POLYGON);
  cout << points[0] << points[1] << points[2] << endl;
  glVertex3f(points[0], points[1], points[2]);
  glVertex3f(points[3], points[4], points[5]);
  glVertex3f(points[3], points[3], points[0]);
  glVertex3f(points[0], points[3], points[0]);
  glEnd();




  glFlush();
  glutSwapBuffers();// swap buffers (we earlier set double buffer)



}


//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void myFrameMove() {
  //nothing here for now
#ifdef _WIN32
  Sleep(10);                                   //give ~10ms back to OS (so as not to waste the CPU)
#endif
  glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}


//****************************************************
// function that handles keyboard callback
//****************************************************
void myKeyboard(unsigned char key, int x, int y) {
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
// MAIN
//****************************************************
int main(int argc, char *argv[]) {
  bool argParse = true;
  // bool argParse = false;

  //*******************************
  // ARGUMENT PARSER
  //*******************************
  if (argParse) {         // hack to turn off until we have input file
  string STRING;
  ifstream infile;
  cout << argv[1] << endl;   //that's input file
  infile.open (argv[1]);
  int numpatches = 0;
  if (!infile.eof()) {
    getline(infile,STRING);
    numpatches = atoi(STRING.c_str());
  }

  if (numpatches == 0) return 1; // throw an exception somehow
  int line = 0;

  // TODO-fix: extra lines at end of file? check if STRING is empty?
  while(!infile.eof()) {      // Each iteration is one LINE <<<<<<
    getline(infile,STRING);
    cout << STRING << endl;
    GLfloat a, b, c, d, e, f, g, h, i, j, k, l;
    std::istringstream iss (STRING);
    iss >> std::skipws >> a >> b >> c;
    iss >> std::skipws >> d >> e >> f;
    iss >> std::skipws >> g >> h >> i;
    iss >> std::skipws >> j >> k >> l;
    // cout << "iteration" << endl;
    // cout << a << b << c << endl;
    // cout << d << e << f << endl;
    // cout << g << h << i << endl;
    // cout << j << k << l << endl;

    // TODO-check if need to pass pointers to the floats...like the vector problem of yore
    /*
    std::vector<GLfloat> point1; 
    point1.push_back(a);
    point1.push_back(b);
    point1.push_back(c);
    std::vector<GLfloat> point2; 
    point2.push_back(d);
    point2.push_back(e);
    point2.push_back(f);
    std::vector<GLfloat> point3; 
    point3.push_back(g);
    point3.push_back(h);
    point3.push_back(i);
    std::vector<GLfloat> point3; 
    point4.push_back(j);
    point4.push_back(k);
    point4.push_back(l);
    std::vector<std::vector<GLfloat>> line;
    line.push_back(point1);
    line.push_back(point2);
    line.push_back(point3);
    line.push_back(point4);
    */


    // glClear(GL_COLOR_BUFFER_BIT);                // clear the color buffer (sets everything to black)
    // glMatrixMode(GL_MODELVIEW);                  // indicate we are specifying camera transformations
    // glLoadIdentity();
    // glColor3f(1.0f,0.0f,0.0f); 

    // glBegin(GL_POLYGON); 
    // glVertex3f(a, b, c);
    // glVertex3f(d, e, f);
    // glVertex3f(g, h, i);
    // glVertex3f(j, k, l);
    // glEnd();
    // glFlush();
    // glutSwapBuffers();  

    // points hack
    points.push_back(a);
    points.push_back(b);
    points.push_back(c);
    points.push_back(d);
    points.push_back(e);
    points.push_back(f);
    points.push_back(g);
    points.push_back(h);
    points.push_back(i);
    points.push_back(j);
    points.push_back(k);
    points.push_back(l);

     

    line++;
    if (line == 4) {
      cout << "/~~~~~~~~~ end patch ~~~~~~~~~~/" << endl;
      getline(infile, STRING);
      line = 0;
    }
    // cout << j << " THATS j " << k << " k " << l << " and l" << endl;
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

}

  //*******************************
  // GLUT STUFF
  //*******************************

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 200;
  viewport.h = 200;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();      // quick function to set up scene

  glutDisplayFunc(myDisplay);     // function to run when its time to draw something
  glutReshapeFunc(myReshape);     // function to run when the window gets resized
  glutIdleFunc(myFrameMove);
  glutKeyboardFunc(myKeyboard);           // function to run when spacebar is pressed: should exit
  glutSpecialFunc(specialKeys);

  glutMainLoop();   // infinite loop that will keep drawing and resizing

  return 0;
}



  //*******************************
  // PSEUDOCODE
  //*******************************
  
  /*****
  for each PATCH in file
    data structure to hold four vertices
    // iterate thru u
    //   iterate thru v
    subdivide(patch, step)
      bezpatchinterp(patch,u,v)
        bezpatchinterp(curve,u);


*/







