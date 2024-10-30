LIBS=-lsfml-graphics -lsfml-window -lsfml-system

all: mainFile
	g++ main.o -o main $(LIBS)
	

mainFile:
	g++ -c main.cpp -o main.o




