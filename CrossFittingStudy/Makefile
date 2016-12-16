rootlibs = $(shell root-config --libs)
rootflags = $(shell root-config --cflags)
localdir = /Users/kimsiang/Work/UWCENPA/gm2/SLAC2016/CrossFittingStudy
templatefitterinc = -I$(localdir)/include
templatefittersrc = $(localdir)/srcs/TemplateFitter.cxx
appliersrc = $(localdir)/srcs/TemplateFitApplier.cc
applierinc = -I$(localdir)/include
applierheader = $(localdir)/include/TemplateFitApplier.hh
warnings = -Wall -Wextra
eigen = -I/usr/local/include/eigen3

all : crossFitter

crossFitter: srcs/crossFitter.cxx $(appliersrc) $(applierheader)
	g++ -o $@ srcs/crossFitter.cxx $(warnings) $(templatefittersrc) $(appliersrc) $(applierinc) -Isrc $(rootflags) -O3 $(templatefitterinc) $(eigen) $(rootlibs)
