rootlibs = $(shell root-config --libs)
rootflags = $(shell root-config --cflags)

warnings = -Wall -Wextra

all : eventViewer 

eventViewer: src/eventViewer.cxx 
	g++ -o $@ src/eventViewer.cxx $(warnings) -Isrc $(rootflags) -ggdb -std=c++0x $(rootlibs)
