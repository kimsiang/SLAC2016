#include "TimeStudyAna.h"
#include <iostream>
#include <numeric> 

void TimeStudyAna::Loop(string filename){

    initialize(filename);

    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();
    Int_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        execute();

    }

    finalize(filename);

}
void TimeStudyAna::initialize(string &filename){

    cout << "Initializing! Analyzing " << filename << endl;

    file = new TFile("testrun.root","recreate");  

    for(int i=0;i<54;i++){

        TString title0(Form("fittedSyncTime%02d",i));
        fittedSyncTime[i] = new TH1D(title0,title0,500,800,900);
        fittedSyncTime[i]->GetXaxis()->SetTitle("time [c.t.]");

        TString title1(Form("syncTime%02d",i));
        syncTime[i] = new TH1D(title1,title1,500,800,900);
        syncTime[i]->GetXaxis()->SetTitle("time [c.t.]");

        TString title2(Form("syncEnergy%02d",i));
        syncEnergy[i] = new TH1D(title2,title2,100,0,3000);
        syncEnergy[i]->GetXaxis()->SetTitle("energy [#photons]");

        for(int j=0;j<54;j++){
            TString title7(Form("syncTimeDiff%02d%02d",i,j));
            syncTimeDiff[i][j] = new TH1D(title7,title7,300,-3,3);
            syncTimeDiff[i][j]->SetCanExtend(kTRUE);
        }

    }

    syncTime2D = new TH2D("syncTime2D","syncTime2D",9,-9,0,6,0,6);
    syncEnergy2D = new TH2D("syncEnergy2D","syncEnergy2D",9,-9,0,6,0,6);

}

void TimeStudyAna::execute(){

    //    cout << "--> execute(), EventNum: " << EventNum <<endl;

    double syncT[54]={};
    double syncE[54]={};
    double fitT[54]={};
    double fitE[54]={};


    // Collect everything for the sync pulses

    for(size_t iFit =0;iFit<FitResult_EventNum->size();iFit++){  
        if(FitResult_Time->at(iFit) < 1000) {
            fittedSyncTime[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Time->at(iFit));
            syncT[FitResult_XtalNum->at(iFit)] = FitResult_Time->at(iFit); 
            syncE[FitResult_XtalNum->at(iFit)] = FitResult_Energy->at(iFit); 
            syncTime[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Time->at(iFit));
            syncEnergy[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Energy->at(iFit));
        }
    }

    for(int i=0;i<54;i++){
        for(int j=0;j<54;j++){
            syncTimeDiff[i][j]->Fill(syncT[i]-syncT[j]);
        } 
    } 

}

void TimeStudyAna::finalize(string &filename){

    double mean = syncTimeDiff[14][23]->GetMean();
    double rms = syncTimeDiff[14][23]->GetRMS();

    TF1 *fit = new TF1("fit","gaus(0)",mean-2,mean+2);
    fit->SetParameters(1000,mean,rms);

    TF1 *fitd = new TF1("fitd","gaus(0)+gaus(3)",mean-2,mean+2);
    fitd->SetParameters(200,-1,0.05,200,1,0.05);

    fitd->SetParLimits(0,10,800);
    fitd->SetParLimits(1,-2,0);
    fitd->SetParLimits(2,0,0.08);
    fitd->SetParLimits(3,10,800);
    fitd->SetParLimits(4,0,2);
    fitd->SetParLimits(5,0,0.08);

    syncTimeDiff[14][23]->Fit("fit","QMER");



    for(int i=0;i<54;i++){

        syncTime2D -> Fill (-i%9-0.5, i/9, syncTime[i]->GetMean());
        syncEnergy2D -> Fill (-i%9-0.5, i/9, syncEnergy[i]->GetMean());
    }



    //   cout << "\tfinalize(): " << filename<< endl;
    file->Write();
    file->Close();

}
