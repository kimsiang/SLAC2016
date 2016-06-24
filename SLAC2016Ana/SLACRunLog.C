#include "SLACRunLog.h"
#include <iostream>
#include <assert.h>

SLACRunLog::SLACRunLog(){

    myfile = new TFile("RunLog.root");
    runTree = (TTree*)myfile->Get("RunLog");
    runTree->SetBranchAddress("run",&runNum);
    runTree->SetBranchAddress("startTime",&startTime);
    runTree->SetBranchAddress("quality",&quality);
    runTree->SetBranchAddress("comment",&comment);
    runTree->SetBranchAddress("crew",&crew);
    runTree->SetBranchAddress("beamE",&beamE);
    runTree->SetBranchAddress("tableX",&tableX);
    runTree->SetBranchAddress("tableY",&tableY);
    runTree->SetBranchAddress("angle",&angle);
    runTree->SetBranchAddress("filterWheel",&filterWheel);
    runTree->SetBranchAddress("bv1",&bv1);
    runTree->SetBranchAddress("bv2",&bv2);
    runTree->SetBranchAddress("bv3",&bv3);
    runTree->SetBranchAddress("bv4",&bv4);
    runTree->SetBranchAddress("stopTime",&stopTime);
    runTree->SetBranchAddress("nEvents",&nEvents);
    runTree->SetBranchAddress("fileSize",&fileSize);
    runTree->SetBranchAddress("rate",&rate);

    if(runTree->GetEntries()){
	runTree->GetEntry(0);
    }

    else{
	throw "The Runlog is empty!!!\n";
    }

}

bool SLACRunLog::GetRunLog(Int_t iRun){

    assert (iRun > 2);

    Int_t iEntry = iRun;
    bool logExist = true;

    while(iRun != runNum){ 
	iEntry--;
	runTree->GetEntry(iEntry);
	if(iRun - runNum > 50) {cout<<"Runlog do not exist!"<<endl; logExist = false; break;}
    }

    return logExist;

}
