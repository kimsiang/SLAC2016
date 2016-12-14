#ifndef CosmicRayAna_h
#define CosmicRayAna_h

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

class CosmicRayAna : public EventCollection {
    public:
	CosmicRayAna(TTree *tree=0):EventCollection(tree){};
	~CosmicRayAna() {};
	virtual void Loop(string filename);
	void initialize(string &filename);
	void execute();
	void finalize(string &filename);
	void clear();

    private:
	TFile *file;     
        TH1D* timeDist;

};
#endif
