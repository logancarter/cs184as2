CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX -Ieigen/
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin -Ieigen/ -I/usr/X11R6/include -w
	LDFLAGS = -lglut -lGLU 
endif
	
RM = /bin/rm -f 
all: main 
main: raytracer.o Util.o
	$(CC) $(CFLAGS) -o raytracer raytracer.o Util.o $(LDFLAGS) -O2 -lm -lpthread -L/opt/X11/lib -lm -lpthread -lX11 -I/usr/X11R6/include
raytracer.o: raytracer.cpp Util.h
	$(CC) $(CFLAGS) -c raytracer.cpp -o raytracer.o -O2 -lm -lpthread -L/opt/X11/lib -lm -lpthread -lX11 -I/usr/X11R6/include
# Vectorz.o: Vectorz.cpp Vectorz.h
	# $(CC) $(CFLAGS) -c Vectorz.cpp -o Vectorz.o
Util.o: Util.cpp Util.h
	$(CC) $(CFLAGS) -c Util.cpp -o Util.o
clean: 
	$(RM) *.o raytracer
 


# g++ -o main main.cpp -O2 -lm -lpthread -L/opt/X11/lib -lm -lpthread -lX11 -I/usr/X11R6/include