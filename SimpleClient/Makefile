EXENAME = SCParser

OBJS = scparser.o

CXX           = g++
ROOTINCS      = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --glibs)
CXXFLAGS      = -g -std=c++0x -Wall -fPIC $(ROOTINCS)

all: $(OBJS)
	$(CXX) -o $(EXENAME) $(OBJS) $(ROOTLIBS)

%.o:%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXENAME) *~

