#ifndef SLAC2016Ana_h
#define SLAC2016Ana_h

#include "EventCollection.h"
#include <stdio.h>
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include <iostream>
#include <fstream>
#include <map>

class SLAC2016Ana : public EventCollection {
    public:
	SLAC2016Ana(TTree *tree=0):EventCollection(tree){};
	~SLAC2016Ana() {};
	virtual void Loop(string filename);
	void initialize(string &filename);
	void execute();
	void finalize(string &filename);
	void clear();

    private:
	TFile *file;     

};
#endif
