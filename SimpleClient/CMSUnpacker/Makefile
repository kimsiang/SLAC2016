EXENAME = Unpack

OBJS = CMSUnpacker.o FedEvent.o FedAMC.o FedBlock.o Fedcrc16d64.o FedException.o FedException_StackTrace.o

CXX           = g++
ROOTINCS      = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --glibs)
CXXFLAGS      = -g -std=c++11 -Wall -fPIC $(ROOTINCS)

all: $(OBJS)
	$(CXX) -o $(EXENAME) $(OBJS) $(ROOTLIBS)

%.o:%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXENAME) *~

