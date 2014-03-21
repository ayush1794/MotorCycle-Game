CC = g++
PROG = game
UTIL = utility
SHAPES = shapes
VEC = vec3f
ILOADER = imageloader
LIBS = -lglut -lGL -lGLU -lopenal -lalut

all: $(PROG)

$(PROG):	$(PROG).o $(UTIL).o $(SHAPES).o $(VEC).o $(ILOADER).o
	$(CC)  -o $(PROG) $(PROG).o $(UTIL).o $(SHAPES).o $(VEC).o $(ILOADER).o $(LIBS)

$(PROG).o:	$(PROG).cpp
	$(CC) -c $(PROG).cpp

$(UTIL).o:	$(UTIL).cpp
	$(CC) -c $(UTIL).cpp

$(SHPAES).o:	$(SHAPES).cpp
	$(CC) -c $(SHAPES).cpp

$(VEC).o:	$(VEC).cpp
	$(CC) -c $(VEC).cpp

$(ILOADER).o:	$(ILOADER).cpp
	$(CC) -c $(ILOADER).cpp

clean:
	rm -rf  *.o $(PROG)
