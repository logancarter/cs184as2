#include "Util.h"
#include "CImg.h" 

using namespace cimg_library; 

int main () {
	CImg<unsigned char> image("milla.bmp"), visu(500,400,1,3,0); 
	image.display("Hello, milla!");
	return 0;
}
