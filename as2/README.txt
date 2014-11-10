Logan Carter/cs184-cx
Jerry Lung/cs184-cv
Our code runs on Mac OSX, needs X11 in opt/X11
Location of code: Logan Carter’s account (cs184-cx)
submitted on hive machine
all website images are in the tests/ folder

To run:
make
./raytracer [FILE]

To our dear Reader/TA:
The spec says PPM is okay, but no indexed color format. So i am submitting PPM file outputs, which of course can't be displayed unless you have certain programs. For that, I have included the BMP counterparts, which our website also uses. The code and pictures are exactly the same - to verify please see Util.cpp in Film::displayToScreen() (line 641-642):

	image.save("file.ppm");
	// image.save("file.bmp");

Please comment out the ppm line and uncomment the bmp line. Remake and rejoice.

Examples:
./raytracer images/input-01.txt
./raytracer images/input-02.txt
... up to 05

Example images from piazza:
./raytracer scene3.txt
./raytracer twospheres.txt
./raytracer sampletri.txt