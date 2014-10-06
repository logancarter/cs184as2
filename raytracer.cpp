#include <iostream>
#include <Eigen/Dense>
#include "Vectorz.h"

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


int main(int argc, char *argv[]) {

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