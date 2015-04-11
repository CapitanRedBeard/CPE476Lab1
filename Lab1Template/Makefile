

osx:
	g++ -ansi -pedantic -o a3 -Wno-deprecated *.cpp *.cc -framework GLUT -framework OpenGL

all:
	g++ -Wno-deprecated *.cpp *.cc -DGL_GLEXT_PROTOTYPES -lglut -lGL -lGLU

clean:
	rm -f *~ *.o a.out
