#include "Util.h"
#include <Eigen/Dense>
#include "CImg.h" 

using namespace cimg_library; 

int main () {
	CImg<unsigned char> image("lena.jpg"), visu(500,400,1,3,0); 
	image.display();
	return 0;
}