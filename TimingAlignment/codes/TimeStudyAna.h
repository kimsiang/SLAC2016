#ifndef TimeStudyAna_h
#define TimeStudyAna_h

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

class TimeStudyAna : public EventCollection {
    public:
	TimeStudyAna(TTree *tree=0):EventCollection(tree){};
	~TimeStudyAna() {};
	virtual void Loop(string filename);
	void initialize(string &filename);
	void execute();
	void finalize(string &filename);
	void clear();

    private:
	TFile *file;     
        TH1D *syncTime[54];       
        TH1D *fittedSyncTime[54];       
        TH1D *syncEnergy[54];       
        TH2D *syncTimeEnergy[54];       
        TH1D *syncTimeDiff[54][54];       
        
        TH1D *fittedBeamTime[54];       
        TH1D *fittedBeamTimeDiff[54][54];       

        TH1D *beamTime[54];       
        TH1D *beamEnergy[54];       
        TH2D *beamTimeEnergy[54];       
        TH1D *beamTimeDiff[54][54];               

        TH2D *syncTime2D;
        TH2D *syncEnergy2D;
        TH2D *beamTime2D;
        TH2D *beamEnergy2D;

        TH2D *beamTimeDiffEnergy[54][54];
        TH2D *syncTimeDiffEnergy[54][54];

        TH1D *chi2;
        TH2D *energyChi2;
};
#endif
