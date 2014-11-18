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

class Patch {
public:
  Vector3f patch[4][4];
  Patch() {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        patch[i][j] = Vector3f(0.0, 0.0, 0.0);
      }
    }
  }
  void printPatch() {
    for (int i = 0; i < 4; i++) {
        cout << patch[i][0].transpose() << " " << patch[i][1].transpose() << " " <<  patch[i][2].transpose() << " " <<  patch[i][3].transpose() << " " << endl;
    }
  }
};

//****************************************************
// Global Variables
//****************************************************

Viewport viewport;
float sub_div_param;
bool adaptive = false;      //if true: adaptive; if false: uniform
std::vector< Patch * > patchez;

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

 Vector3f bezcurveinterp(Vector3f* zero, Vector3f* one, Vector3f* two, Vector3f* three, GLfloat u, Vector3f *dPdu) {
  cout << "bezcurve " << (*zero).transpose() << " " << (*one).transpose() << " " << (*two).transpose() << " " << (*three).transpose() << " U_step " << u << endl;
  Vector3f a = *zero * (1.0 - u) + *one * u;
  Vector3f b = *one * (1.0 - u) + *two * u;
  Vector3f c = *two * (1.0 - u) + *three * u;

  Vector3f d = a * (1.0 - u) + b * u;
  Vector3f e = b * (1.0 - u) + c * u;

  Vector3f p = d * (1.0 - u) + e * u;

  Vector3f fordpdu = 3 * (e - d);
  *dPdu << fordpdu[0], fordpdu[1], fordpdu[2];


  // cout << dPdu.z() << " HELLO" << endl;
  // cout << curve[0].x << endl;
  // cout << curve[0].y << endl;
  // cout << curve[0].z << endl;
  // cout << p.x() << " " << p.y() << " " << p.z() << "in function" << endl;

  cout << "bezcurve returns " << p.transpose() << endl;
  return p;
}


Vector3f bezpatchinterp(Patch &patch, GLfloat &u, GLfloat &v, Vector3f* n) {
// Vector3f bezpatchinterp(vector<vector<Vector3f*> > patch, float u, float v, Vector3f *n) {
  cout << "bezpatch u: " << u << " v: " << v << endl;
  vector<Vector3f> vcurve;
  vector<Vector3f> ucurve;
  // Vector3f p = *(new Vector3f(0.0,0.0,0.0));
  Vector3f p;
  Vector3f *dPdu = new Vector3f(0.0,0.0,0.0);
  Vector3f *dPdv = new Vector3f(0.0,0.0,0.0);
  // cout << "first row: " << patch.patch[0][0].transpose() << " " << patch.patch[0][1].transpose() << " " << patch.patch[0][2].transpose() << " " << patch.patch[0][3].transpose() << endl;
  vcurve.push_back(bezcurveinterp(&patch.patch[0][0], &patch.patch[0][1], &patch.patch[0][2], &patch.patch[0][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[1][0], &patch.patch[1][1], &patch.patch[1][2], &patch.patch[1][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[2][0], &patch.patch[2][1], &patch.patch[2][2], &patch.patch[2][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[3][0], &patch.patch[3][1], &patch.patch[3][2], &patch.patch[3][3], u, dPdu));

  ucurve.push_back(bezcurveinterp(&patch.patch[0][0], &patch.patch[1][0], &patch.patch[2][0], &patch.patch[3][0], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][1], &patch.patch[1][1], &patch.patch[2][1], &patch.patch[3][1], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][2], &patch.patch[1][2], &patch.patch[2][2], &patch.patch[3][2], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][3], &patch.patch[1][3], &patch.patch[2][3], &patch.patch[3][3], v, dPdu));

  cout << vcurve[0].transpose() << " "<< vcurve[1].transpose() << " "<< vcurve[2].transpose() << " " << vcurve[3].transpose() << endl;
  Vector3f v_point = bezcurveinterp(&vcurve[0], &vcurve[1], &vcurve[2], &vcurve[3], v, dPdv);
  cout << "v curve: " << v_point.transpose() << endl;
  // cout << "p " << p.transpose() << endl;
  Vector3f u_point = bezcurveinterp(&ucurve[0], &ucurve[1], &ucurve[2], &ucurve[3], u, dPdu);
  // cout << "p " << p.transpose() << endl;
  cout << "u curve: " << u_point.transpose() << endl;
  p = v_point;

  // cout << "dpdu " << *dPdu << endl;
  // cout << "dpdv " << *dPdv << endl;

  Vector3f n2 = (*dPdu).cross(*dPdv);

  // cout << "n2 : " << n2 << endl;

  *n = n2 / n2.norm();

  // cout << "n2 normalized: " << *n << endl;
  cout << "===== P  ======" << endl;
  cout << p << endl;
  return p;
}


void subdividepatch(Patch &patch, GLfloat step) {
// void subdividepatch(vector<vector<Vector3f*> > patch, float step) {
// Vector3f subdividepatch(vector<vector<Vector3f*> > patch, float step) {
  // patch.printPatch();
  Vector3f* n = new Vector3f(0.0, 0.0, 0.0);
  GLfloat epsilon = step - fmod(1.0, step);
  GLfloat numdiv = ((1 + epsilon) / step);
  cout << "numdiv " << numdiv << endl;
  // TODO: is it ++?
  for (int iu = 0; iu < numdiv; iu ++) {
    GLfloat u = iu * step;
    cout << "iu " << iu << endl;
    for (int iv = 0; iv < numdiv; iv++) {
      cout << "iv " << iv << endl;
      GLfloat v = iv * step;
      cout << "subdivide: u: " << u << " v: " << v << endl;
      Vector3f p = bezpatchinterp(patch, u, v, n);
      //TODO:save surface point and normal
      cout << p << endl;
      // cout << *n << endl;
      cout << "---- end -----" << endl;
      somepoints_toconnect.push_back(p);
      // pointsofcurves.push_back(somepoints_toconnect);
    }
  }
}


//****************************************************
// MAIN
//****************************************************
int main(int argc, char *argv[]) {
  // std::vector<Vector3f**> patchez;

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

  if (numpatches == 0) return 1; // throw an exception somehow]
  
  // TODO: Assume input is well-formed!!!
  for (int i = 0; i < numpatches; i++) {
    Patch* current_patch = new Patch();
    int line = 0;


    // TODO-fix: extra lines at end of file? check if STRING is empty?
    while(!infile.eof() && line < 4) {      // Each iteration is one LINE <<<<<<
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
      // cout << curves.size() << "   SIZE" << endl;



      // cout << "line " << line << " done" << endl;
      current_patch->patch[line][0] = *apoint;
      current_patch->patch[line][1] = *bpoint;
      current_patch->patch[line][2] = *cpoint;
      current_patch->patch[line][3] = *dpoint;
      // cout << "line " << line << " done" << endl;

      line++;

    } // while

    cout << "/~~~~~~~~~ end patch ~~~~~~~~~~/" << endl;
    // getline(infile, STRING);
    patchez.push_back(current_patch);

  } // for

  // cout << "patchez size: " << patchez.size() << endl;
  // for (int l = 0; l < patchez.size(); l++) {
  //   patchez[l].printPatch();
  // }

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


  // Vector3f no;
  // for (int k = 0; k < curves.size(); k++) {
  //   vector<Vector3f> somepoints_toconnect;
  //   for (float j = 0; j < 1; j += sub_div_param) {
  //     Vector3f result = bezcurveinterp(curves[k][0], curves[k][1], curves[k][2], curves[k][3], j, no);
  //     somepoints_toconnect.push_back(result);
  //   }
  //   pointsofcurves.push_back(somepoints_toconnect);
  // }

  Vector3f normal;
  // for (int k = 0; k < patchez.size(); k++) {
  for (int k = 0; k < 1; k++) {
    vector<Vector3f> somepoints_toconnect;
    // Uniform
    cout << "on patch " << k << endl;
    /*
    for (GLfloat j = 0; j < 1; j += sub_div_param) {
      // TODO: save normals
      Vector3f result = subdividepatch(*patchez[k], j);
      cout << result.transpose() << endl;
      somepoints_toconnect.push_back(result);
    }
    */
    subdividepatch(*patchez[k], sub_div_param);
    // cout << result.transpose() << endl;
    // somepoints_toconnect.push_back(result);
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









