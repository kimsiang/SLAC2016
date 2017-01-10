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

    file = new TFile(Form("%s.root",filename.c_str()),"recreate");  

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
	    syncTimeDiff[i][j] = new TH1D(title7,title7,600,-3,3);
	    //       syncTimeDiff[i][j]->SetCanExtend(kTRUE);
	}

    }

    syncTime2D = new TH2D("syncTime2D","syncTime2D",9,-9,0,6,0,6);
    syncEnergy2D = new TH2D("syncEnergy2D","syncEnergy2D",9,-9,0,6,0,6);

}

void TimeStudyAna::execute(){

    //    cout << "--> execute(), EventNum: " << EventNum <<endl;

    double syncT[54]={};
    //   double syncE[54]={};


    // Collect everything for the sync pulses

    for(size_t iFit =0;iFit<FitResult_EventNum->size();iFit++){  
	if(FitResult_Time->at(iFit) < 1000) {
	    fittedSyncTime[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Time->at(iFit));
	    syncT[FitResult_XtalNum->at(iFit)] = FitResult_Time->at(iFit); 
	    // syncE[FitResult_XtalNum->at(iFit)] = FitResult_Energy->at(iFit); 
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

    const int channel_i[4] = {9,18,27,36};
    const int channel_j = 0;


    for(int i=0;i<4;i++){

	double mean = syncTimeDiff[channel_i[i]][channel_j]->GetMean();
	double rms = syncTimeDiff[channel_i[i]][channel_j]->GetRMS();

	TF1 *fit = new TF1("fit","gaus(0)",mean-2,mean+2);
	fit->SetParameters(1000,mean,rms);

	syncTimeDiff[channel_i[i]][channel_j]->Fit("fit","QMER");
	cout<<"mean["<< channel_i[i]<<"]["<< channel_j<<"]: "<< fit->GetParameter(1)<<", rms: "<<fit->GetParameter(2)<<endl;
	delete fit; 

    }


    for(int i=0;i<54;i++){

	syncTime2D -> Fill (-i%9-0.5, i/9, syncTime[i]->GetMean());
	syncEnergy2D -> Fill (-i%9-0.5, i/9, syncEnergy[i]->GetMean());
    }



    //   cout << "\tfinalize(): " << filename<< endl;
    file->Write();
    file->Close();

}
