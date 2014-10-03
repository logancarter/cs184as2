CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX -Ieigen/
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin -Ieigen/
	LDFLAGS = -lglut -lGLU
endif
	
RM = /bin/rm -f 
all: main 
main: raytracer.o Vectorz.o
	$(CC) $(CFLAGS) -o raytracer raytracer.o Vectorz.o $(LDFLAGS) 
raytracer.o: raytracer.cpp Vectorz.h
	$(CC) $(CFLAGS) -c raytracer.cpp -o raytracer.o
Vectorz.o: Vectorz.cpp Vectorz.h
	$(CC) $(CFLAGS) -c Vectorz.cpp -o Vectorz.o
clean: 
	$(RM) *.o raytracer
 


