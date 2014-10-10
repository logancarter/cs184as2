// test.cpp
#include "CImg.h" 
using namespace cimg_library; 
int main(){ 
//  CImg<double> image("starwars.bmp"); 
//  CImgDisplay main_disp(image,"Image",0); 
//  CImg<double> darkimage(image.dimx(),image.dimy(),1,3,0);
// for (int i=0;i<image.dimx();i++) 
//  for (int j=0;j<image.dimy();j++) 
//  for (int k=0;k<3;k++) 
//  darkimage(i,j,0,k)=image(i,j,0,k)/2; 
//  CImgDisplay dark_disp (darkimage,"Dark Image",0); 
//  while (!main_disp.is_closed) 
//  main_disp.wait(); 
 CImg<unsigned char> image(500,500, 1,3, 0); 
  // CImg<double> image("milla.bmp");
  // CImg<double> visu(image.width(),image.height(),1,3, 0); 
  const double red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  for (int i=0;i<image.width();i++) {
    for (int j=0;j<image.height();j++) {
      for (int k=0;k<3;k++) {
        // visu(i,j,0,k)=image(i,j,0,k)*5; 
        image(i, j, 0, k) = 100;
      }
    }
  }
  // image.display("Hello, milla!");
  image.display("Dark!");
 return 0; 
 }