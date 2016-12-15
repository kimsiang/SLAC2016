rootlibs = $(shell root-config --libs)
rootflags = $(shell root-config --cflags)

warnings = -Wall -Wextra

all : slac_template_builder 

slac_template_builder: srcs/slac_template_builder.cxx 
	g++ -o $@ srcs/slac_template_builder.cxx $(warnings) -Isrc $(rootflags) -ggdb -std=c++0x $(rootlibs)
