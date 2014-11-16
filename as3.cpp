#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
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
using namespace Eigen;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
  int w, h; // width and height
};

//part of algorithm credit goes to stack overflow
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
float sub_div_param;
bool adaptive = false;      //if true: adaptive; if false: uniform
std::vector<int *> patches;

//std::vector<Vector3f *> points;

vector<Vector3f> somepoints_toconnect;
float zoomamount = 1.0;
float horizontalshift = 0.0;
float verticalshift = 0.0;
float rotatehoriz = 0.0;
float rotatevertical = 0.0;
vector<vector<Vector3f*> > curves;
vector<vector<Vector3f> > pointsofcurves;


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch

}

//****************************************************
// Simple init function
//****************************************************
void initScene(){

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
  myReshape(viewport.w,viewport.h);

}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);       // clear the color buffer

  glMatrixMode(GL_MODELVIEW);        // indicate we are specifying camera transformations
  glLoadIdentity();        // make sure transformation is "zero'd"

  glColor3f(1.0f,0.0f,0.0f);                   // setting the color to pure red 90% for the rect
  //gluLookAt(-2, 0, 0, 0, 0,-1, 0, 1, 0);
  glTranslatef(horizontalshift, verticalshift, 0.0);
  glRotatef(rotatevertical, 1, 0, 0);
  glRotatef(rotatehoriz, 0, 1, 0);
  // TODO: this? vvv
  // glTranslatef(-horizontalshift, -verticalshift, -0.0);
  glScalef(zoomamount, zoomamount, zoomamount);
  glLineWidth(3);

  glColor3f(1.0f,0.0f,0.0f); 
  for (int i = 0; i < pointsofcurves.size(); i++) {
    //cout << "one" << endl;
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < pointsofcurves[i].size(); j++) {
      glVertex3f(pointsofcurves[i][j].x(), pointsofcurves[i][j].y(), pointsofcurves[i][j].z());
     // cout << pointsofcurves[i][j].x() <<" " << pointsofcurves[i][j].y() << " " << pointsofcurves[i][j].z() << endl;
    }
  }
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
    zoomamount += .2;
    glutPostRedisplay();
    cout << "+: zoom in" << endl;
    break;
  case '-':
    zoomamount -= .2;
    glutPostRedisplay();
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
      verticalshift += .1;
      glutPostRedisplay();
      cout << "translate: up" << endl;
      break;
    case GLUT_KEY_DOWN:
      verticalshift -= .1;
      glutPostRedisplay();
      cout << "translate: down" << endl;
      break;
    case GLUT_KEY_RIGHT:
      horizontalshift += .1;
      glutPostRedisplay();
      cout << "translate: right" << endl;
      break;
    case GLUT_KEY_LEFT:
      glutPostRedisplay();
      horizontalshift -= .1;
      cout << "translate: left" << endl;
      break;
    default:
      cout << "shift special " << key << endl;
      break;
    }
  } else {
    switch (key) {
    case GLUT_KEY_UP:
      rotatevertical += 10;
      glutPostRedisplay();
      cout << "rotate: up" << endl;
      break;
    case GLUT_KEY_DOWN:
      rotatevertical -= 10;
      glutPostRedisplay();
      cout << "rotate: down" << endl;
      break;
    case GLUT_KEY_RIGHT:
      rotatehoriz += 10;
      glutPostRedisplay();
      cout << "rotate: right" << endl;
      break;
    case GLUT_KEY_LEFT:
      rotatehoriz -= 10;
      glutPostRedisplay();
      cout << "rotate: left" << endl;
      break;
    default:
      cout << "special " << key << endl;
      break;
    }
  }
}

//credit goes to stack overflow for this function
bool isEmptyOrBlank(const std::string& str) {
   int len = str.size();
   if(len == 0) { return true; }

   for(int i=0;i<len;++i) {
       if(str[i] != ' ' and str[i] != '\n' and str[i] != '\t' and str[i] != '\r') { return false; }
   }
   return true;
}

 Vector3f bezcurveinterp(Vector3f* zero, Vector3f* one, Vector3f* two, Vector3f* three, float u, Vector3f &dPdu) {
  //cout << *curve[0] << " " << *curve[1] << " " << *curve[2] << " beg function" << endl;
  Vector3f a = *zero * (1.0 - u) + *one * u;
  Vector3f b = *one * (1.0 - u) + *two * u;
  Vector3f c = *two * (1.0 - u) + *three * u;

  Vector3f d = a * (1.0 - u) + b * u;
  Vector3f e = b * (1.0 - u) + c * u;

  Vector3f p = d * (1.0 - u) + e * u;

  Vector3f fordpdu = 3 * (e - d);
  dPdu << fordpdu[0], fordpdu[1], fordpdu[2];
  cout << dPdu.z() << " HELLO" << endl;
  // cout << curve[0].x << endl;
  // cout << curve[0].y << endl;
  // cout << curve[0].z << endl;
  cout << p.x() << " " << p.y() << " " << p.z() << "in function" << endl;
  return p;
}

//vector<vector<Vector3f*> > curves;
Vector3f bezpatchinterp(vector<vector<Vector3f*> > patch, float u, float v, Vector3f *n) {
  vector<Vector3f> vcurve;
  vector<Vector3f> ucurve;
  Vector3f p;
  Vector3f dPdu;
  Vector3f dPdv;
  vcurve[0] = bezcurveinterp(patch[0][0], patch[0][1], patch[0][3], patch[0][4], u, dPdu);
  vcurve[1] = bezcurveinterp(patch[1][0], patch[1][1], patch[1][3], patch[1][4], u, dPdu);
  vcurve[2] = bezcurveinterp(patch[2][0], patch[2][1], patch[2][3], patch[2][4], u, dPdu);
  vcurve[3] = bezcurveinterp(patch[3][0], patch[3][1], patch[3][3], patch[3][4], u, dPdu);

  ucurve[0] = bezcurveinterp(patch[0][0], patch[1][0], patch[2][0], patch[3][0], v, dPdu);
  ucurve[1] = bezcurveinterp(patch[0][1], patch[1][1], patch[2][1], patch[3][1], v, dPdu);
  ucurve[2] = bezcurveinterp(patch[0][2], patch[1][2], patch[2][2], patch[3][2], v, dPdu);
  ucurve[3] = bezcurveinterp(patch[0][3], patch[1][3], patch[2][3], patch[3][3], v, dPdu);

  p = bezcurveinterp(&vcurve[0], &vcurve[1], &vcurve[2], &vcurve[3], v, dPdv);
  p = bezcurveinterp(&ucurve[0], &ucurve[1], &ucurve[2], &ucurve[3], u, dPdu);

  Vector3f n2 = dPdu.cross(dPdv);
  *n = n2 / n2.norm();
  return p;
}

void subdividepatch(vector<vector<Vector3f*> > patch, float step) {
  Vector3f* n;
  float numdiv = ((1 + .01) / step);
  for (int iu = 0; iu < numdiv; iu ++) {
    float u = iu * step;
    for (int iv = 0; iv < numdiv; iv++) {
      float v = iv * step;
      Vector3f p = bezpatchinterp(patch, u, v, n);
      //TODO:save surface point and normal
    }
  }
}

//****************************************************
// MAIN
//****************************************************
int main(int argc, char *argv[]) {

  //*******************************
  // ARGUMENT PARSER
  //*******************************
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
    if (isEmptyOrBlank(STRING)) {
      continue;
    }
    cout << STRING << endl;
    GLfloat a, b, c, d, e, f, g, h, i, j, k, l;
    std::istringstream iss (STRING);
    iss >> std::skipws >> a >> b >> c;
    iss >> std::skipws >> d >> e >> f;
    iss >> std::skipws >> g >> h >> i;
    iss >> std::skipws >> j >> k >> l;

    vector<Vector3f*> curve;
    Vector3f *apoint = new Vector3f(a, b, c);
    Vector3f *bpoint = new Vector3f(d, e, f);
    Vector3f *cpoint = new Vector3f(g, h, i);
    Vector3f *dpoint = new Vector3f(j, k, l);
    curve.push_back(apoint);
    curve.push_back(bpoint);
    curve.push_back(cpoint);
    curve.push_back(dpoint);
    curves.push_back(curve);
    cout << curves.size() << "   SIZE" << endl;

    line++;
    if (line == 4) {
      cout << "/~~~~~~~~~ end patch ~~~~~~~~~~/" << endl;
      getline(infile, STRING);
      
      line = 0;
    }

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


  Vector3f no;
  for (int k = 0; k < curves.size(); k++) {
      vector<Vector3f> somepoints_toconnect;
    for (float j = 0; j < 1; j += sub_div_param) {
      //cout << curves[i][j] << " " << curves[i] << " " << curves[i] << " HIHIH" << endl;
      Vector3f result = bezcurveinterp(curves[k][0], curves[k][1], curves[k][2], curves[k][3], j, no);
      //cout << "Nooooooo: " << no.x() << " " << no.y() << " " << no.z() << endl;
      //cout << no.z() << " after" << endl;
      somepoints_toconnect.push_back(result);
    }
  pointsofcurves.push_back(somepoints_toconnect);
}


  //*******************************
  // GLUT STUFF
  //*******************************

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 500;
  viewport.h = 500;

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







