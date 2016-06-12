#ifndef SLACRunLog_h
#define SLACRunLog_h

#include <iostream>
#include <fstream>
#include <string>
#include "TString.h"
#include "TFile.h"
#include <cstdlib>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

using namespace std;

class SLACRunLog{

    public:
	SLACRunLog();
	bool GetRunLog(Int_t iRun); 

	TFile *myfile;
	TTree *runTree;
	Int_t runNum;
	Char_t startTime[100]; 
	Char_t comment[100]; 
	Char_t quality[100]; 
	Char_t crew[100]; 
	Double_t beamE;
	Double_t tableX;
	Double_t tableY;
	Double_t angle;
	Int_t filterWheel;
	Double_t bv1;
	Double_t bv2;
	Double_t bv3;
	Double_t bv4;
	Char_t stopTime[100]; 
	Int_t nEvents; 
	Int_t fileSize; 
	Double_t rate;

};

#endif // #ifdef SLACRunLog_cxx
