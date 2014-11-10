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

class Vectorz
{
  GLfloat x, y, z;
public:
  Vectorz() {x = y = z = 0;}
  // virtual ~Vectorz(){}
  Vectorz(GLfloat r, GLfloat g, GLfloat b) {x = r; y = g; z = b;}
  GLfloat getX() {return x;}
  GLfloat getY() {return y;}
  GLfloat getZ() {return z;}
  void setX(GLfloat val) {x = val;}
  void setY(GLfloat val) {y = val;}
  void setZ(GLfloat val) {z = val;}
  void setValues(GLfloat x1, GLfloat y1, GLfloat z1) {
    x = x1; y = y1; z = z1;
  }
  Vectorz flip() {
    Vectorz res;
    res.setX(-this->getX());
    res.setY(-this->getY());
    res.setZ(-this->getZ());
    return res;
  }
  GLfloat length() {return sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());}
  Vectorz normalize() {
    Vectorz res;
    res.setValues(this->getX() / this->length(), this->getY() / this->length(), this->getZ() / this->length());
    return res;
  }
  static GLfloat dot(Vectorz v1, Vectorz v2) {return v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ();}
  static Vectorz add(Vectorz v1, Vectorz v2) {
    Vectorz res;
    res.setX(v1.getX()+v2.getX());
    res.setY(v1.getY()+v2.getY());
    res.setZ(v1.getZ()+v2.getZ());
    return res;
  }
  static Vectorz subtract(Vectorz v1, Vectorz v2) {
    Vectorz res;
    res.setX(v1.getX()-v2.getX());
    res.setY(v1.getY()-v2.getY());
    res.setZ(v1.getZ()-v2.getZ());
    return res;
  }
  static Vectorz elementMult(Vectorz v1, Vectorz v2) {
    Vectorz res;
    res.setX(v1.getX()*v2.getX());
    res.setY(v1.getY()*v2.getY());
    res.setZ(v1.getZ()*v2.getZ());
    return res;
  }
  static Vectorz scale(Vectorz v1, GLfloat val) {
    Vectorz res;
    res.setX(v1.getX()*val);
    res.setY(v1.getY()*val);
    res.setZ(v1.getZ()*val);
    return res;
  }
  void printV() {
    cout << this->getX() << " " << this->getY() << " " << this->getZ() << endl;
  }

};
// Vectorz::Vectorz() {
//         x = y = z = 0;
//       }
// Vectorz::Vectorz(GLfloat r, GLfloat g, GLfloat b) {
//         x = r; y = g; z = b;
//       }

class Light {
public:
  GLfloat r, g, b, x, y, z;
  void setValues(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat r1, GLfloat g1, GLfloat b1) {
    x = x1; y = y1; z = z1; r = r1; g = g1; b = b1;
  }
  virtual int isDLight() { return 30; }
};

class PointLight: public Light {
public:
  int isDLight() {
    return 0;
  }
};

class DirectionalLight: public Light {
public:
  int isDLight() {
    return 1;
  }
};

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;

enum string_code {
  ka,
  kd,
  ks,
  sp,
  pl,
  dl,
  error,
};

// TODO: Make a data structure of the lights, can be dynamically filled
GLfloat ka_r, ka_g, ka_b, kd_r, kd_g, kd_b, ks_r, ks_g, ks_b, sp_v, pl_x, pl_y, pl_z, pl_r, pl_g, pl_b, dl_x, dl_y, dl_z, dl_r, dl_g, dl_b;
int hasAmbient, hasDiffuse, hasSpecular, hasPLight, hasDLight, lightptr;
std::vector<Light *> lights;

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
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);


  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));

  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      if (!hasPLight && !hasDLight) { setPixel(i,j, 0.0, 0.0, 0.0); continue; }

        
      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {

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

        setPixel(i,j, result.getX(), result.getY(), result.getZ());

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }


    }
  }


  glEnd();
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
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) * 0.9 / 2);

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
string_code hashstring (std::string const& inString) {
  if (inString == "-ka") return ka;
  if (inString == "-kd") return kd;
  if (inString == "-ks") return ks;
  if (inString == "-sp") return sp;
  if (inString == "-pl") return pl;
  if (inString == "-dl") return dl;
  return error;
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

  //*******************************
  // ARGUMENT PARSER
  //*******************************

  hasAmbient = hasDiffuse = hasSpecular = hasPLight = hasDLight = lightptr = 0;

  for (int i = 1; i < argc; i++) {
    // cout<<"iterate: " <<argv[i]<<"\n";
    string curr = argv[i];
    // TODO: Can we assume that the arguments will be called correctly?
    switch(hashstring(curr)) {
    case ka: 
      // cout << "ambient" << endl;
      hasAmbient = 1;
      ka_r = (GLfloat) atof(argv[i+1]);
      ka_g = (GLfloat) atof(argv[i+2]);
      ka_b = (GLfloat) atof(argv[i+3]);
      // cout << ka_r << ka_g << ka_b << endl;
      i += 3;
      break;
    case kd: 
      // cout << "diffuse" << endl; 
      hasDiffuse = 1;
      kd_r = (GLfloat) atof(argv[i+1]);
      kd_g = (GLfloat) atof(argv[i+2]);
      kd_b = (GLfloat) atof(argv[i+3]);
      i += 3;
      // cout << kd_r << kd_g << kd_b << endl;
      break;
    case ks: 
      // cout << "specular" << endl; 
      hasSpecular = 1;
      ks_r = (GLfloat) atof(argv[i+1]);
      ks_g = (GLfloat) atof(argv[i+2]);
      ks_b = (GLfloat) atof(argv[i+3]);
      i += 3;
      // cout << ks_r << ks_g << ks_b << endl;
      break;
    case sp: 
      // cout << "power coefficient" << endl; 
      sp_v = (GLfloat) atof(argv[i+1]);
      i += 1;
      // cout << sp_v << endl;
      break;
    case pl: {
      cout << "point light" << endl;
      hasPLight = 1;
      pl_x = (GLfloat) atof(argv[i+1]);
      pl_y = (GLfloat) atof(argv[i+2]);
      pl_z = (GLfloat) atof(argv[i+3]);
      pl_r = (GLfloat) atof(argv[i+4]);
      pl_g = (GLfloat) atof(argv[i+5]);
      pl_b = (GLfloat) atof(argv[i+6]);
      i += 6;
      PointLight *plight = new PointLight();
      (*plight).setValues(pl_x,pl_y,pl_z,pl_r,pl_g,pl_b);
      lights.push_back(plight);

      lightptr++;
      // cout << pl_x << " " << pl_y << " " << pl_z << " " << pl_r << " " << pl_g << " " << pl_b << endl;
    } break;
    case dl: {
      // cout << "directional light" << endl; 

      hasDLight = 1;
      dl_x = (GLfloat) atof(argv[i+1]);
      dl_y = (GLfloat) atof(argv[i+2]);
      dl_z = (GLfloat) atof(argv[i+3]);
      dl_r = (GLfloat) atof(argv[i+4]);
      dl_g = (GLfloat) atof(argv[i+5]);
      dl_b = (GLfloat) atof(argv[i+6]);
      i += 6;
      DirectionalLight *dlight = new DirectionalLight();
      (*dlight).setValues(dl_x,dl_y,dl_z,dl_r,dl_g,dl_b);
      lights.push_back(dlight);
      lightptr++;
      // cout << dl_x << " " << dl_y << " " << dl_z << " " << dl_r << " " << dl_g << " " << dl_b << endl;
    } break;
    default: cout << "Invalid argument " << argv[i] << endl;
    } 
  }

  cout << hasDLight << hasPLight << endl;


  for (int k = 0; k < lights.size(); k++) {
    Vectorz lightpos;
    lightpos.setValues(lights[k]->x, lights[k]->y, lights[k]->z);
    Vectorz I_rgb;
    I_rgb.setValues(lights[k]->r, lights[k]->g, lights[k]->b);
    lightpos.printV();
    I_rgb.printV();
    // cout << lights[k] << endl;
  }

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
  glutKeyboardFunc(myKeyboard);           // function to run when spacebar is pressed: should exit
  glutSpecialFunc(specialKeys);

  glutMainLoop();// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}

// TODO: Normalize your vectors



// static GLfloat Vector::dot(Vector v1, Vector v2) {
//   return v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ();
// }

// static Vector Vector::subtract(Vector v1, Vector v2) {
//   Vector res;
//   res.setX(v1.getX()-v2.getX());
//   res.setY(v1.getY()-v2.getY());
//   res.setZ(v1.getZ()-v2.getZ());
//   return res;
// }
  
