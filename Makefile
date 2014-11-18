CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX -Ieigen/ -O2
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin -Ieigen/ -w -O2
	LDFLAGS = -lglut -lGLU
endif
	
RM = /bin/rm -f 
all: main 
main: as3.o 
	$(CC) $(CFLAGS) -o as3 as3.o $(LDFLAGS) -w
as3.o: as3.cpp
	$(CC) $(CFLAGS) -c as3.cpp -o as3.o -w
clean: 
	$(RM) *.o as3
 


