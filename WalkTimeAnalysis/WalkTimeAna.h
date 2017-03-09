#ifndef WalkTimeAna_h
#define WalkTimeAna_h

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

class WalkTimeAna : public EventCollection {
    public:
	WalkTimeAna(TTree *tree=0):EventCollection(tree){};
	~WalkTimeAna() {};
	virtual void Loop(string filename);
	void initialize(string &filename);
	void execute();
	void finalize(string &filename);
	void clear();

    private:
TFile *file;     

  TH1D *laserTimes[10];
  TH1D *dt[54];
  TH2D *syncE_dt[54];
};
#endif
