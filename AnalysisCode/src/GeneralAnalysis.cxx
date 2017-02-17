#include "../include/GeneralAnalysis.h"
#include <iostream>
#include <numeric> 

void GeneralAnalysis::Loop(string filename){

  cout<<"In Loop(): "<<filename<<endl;

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

void GeneralAnalysis::initialize(string &filename){

  cout << "initialize()" << endl;

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

    TString title3(Form("syncTimeEnergy%02d",i));
    syncTimeEnergy[i] = new TH2D(title3,title3,100,800,900,100,0,3000);
    syncTimeEnergy[i]->GetXaxis()->SetTitle("time [c.t.]");
    syncTimeEnergy[i]->GetYaxis()->SetTitle("energy [#photons]");

    TString title4(Form("beamTime%02d",i));
    beamTime[i] = new TH1D(title4,title4,500,1300,1700);
    beamTime[i]->GetXaxis()->SetTitle("time [c.t.]");

    TString title5(Form("beamEnergy%02d",i));
    beamEnergy[i] = new TH1D(title5,title5,100,0,3000);
    beamEnergy[i]->GetXaxis()->SetTitle("energy [#photons]");

    TString title6(Form("beamTimeEnergy%02d",i));
    beamTimeEnergy[i] = new TH2D(title6,title6,200,1300,1700,300,0,3000);
    beamTimeEnergy[i]->GetXaxis()->SetTitle("time [c.t.]");
    beamTimeEnergy[i]->GetYaxis()->SetTitle("energy [#photons]");

    for(int j=0;j<54;j++){
      TString title7(Form("syncTimeDiff%02d%02d",i,j));
      syncTimeDiff[i][j] = new TH1D(title7,title7,300,-3,3);
      syncTimeDiff[i][j]->SetCanExtend(kTRUE);

      TString title8(Form("beamTimeDiff%02d%02d",i,j));
      beamTimeDiff[i][j] = new TH1D(title8,title8,100,-1,1);
      beamTimeDiff[i][j]->SetCanExtend(kTRUE);

      TString title9(Form("syncTimeDiffEnergy%02d%02d",i,j));
      syncTimeDiffEnergy[i][j] = new TH2D(title9,title9,100,-0.5,0.5,100,-500,500);
      syncTimeDiffEnergy[i][j]->SetCanExtend(kTRUE);

      TString title10(Form("beamTimeDiffEnergy%02d%02d",i,j));
      beamTimeDiffEnergy[i][j] = new TH2D(title10,title10,100,-0.5,0.5,100,-500,500);
      beamTimeDiffEnergy[i][j]->SetCanExtend(kTRUE);

      TString title11(Form("fittedBeamTimeDiff%02d%02d",i,j));
      fittedBeamTimeDiff[i][j] = new TH1D(title11,title11,100,-1,1);
      fittedBeamTimeDiff[i][j]->SetCanExtend(kTRUE);
    }

    TString title12(Form("fittedBeamTime%02d",i));
    fittedBeamTime[i] = new TH1D(title12,title12,500,2200,2600);
    fittedBeamTime[i]->GetXaxis()->SetTitle("time [c.t.]");

  }

  syncTime2D = new TH2D("syncTime2D","syncTime2D",9,-9,0,6,0,6);
  beamTime2D = new TH2D("beamTime2D","beamTime2D",9,-9,0,6,0,6);
  syncEnergy2D = new TH2D("syncEnergy2D","syncEnergy2D",9,-9,0,6,0,6);
  beamEnergy2D = new TH2D("beamEnergy2D","beamEnergy2D",9,-9,0,6,0,6);

  chi2 = new TH1D("chi2","chi2",200,0,500);
  energyChi2 = new TH2D("energyChi2","energyChi2",200,0,12000,200,0,500);
}

void GeneralAnalysis::execute(){

  cout << "--> execute(), EventNum: " << EventNum <<endl;

  // store the sync time and energy, and beam time and energy in each crystal in each event
  double syncT[54]={};
  double syncE[54]={};
  double beamT[54]={};
  double beamE[54]={};
  double fitT[54]={};
  double fitE[54]={};

  // Collect everything for the sync pulses before correction (aka raw Fit Result)
  for(size_t iFit =0;iFit<FitResult_EventNum->size();iFit++){  
    if(FitResult_Time->at(iFit) < 1000) {
      fittedSyncTime[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Time->at(iFit));
      syncT[FitResult_XtalNum->at(iFit)] = FitResult_Time->at(iFit); 
    }
  }

  // Collect everything for the sync pulses after correction (aka calibration xtal hit)
  for(size_t iF =0;iF<XtalHit_EventNum->size();iF++){  
    if(XtalHit_Time->at(iF) < 1000) {
      syncE[XtalHit_XtalNum->at(iF)] = XtalHit_Energy->at(iF); 
      syncTime[XtalHit_XtalNum->at(iF)]->Fill(XtalHit_Time->at(iF));
      syncEnergy[XtalHit_XtalNum->at(iF)]->Fill(XtalHit_Energy->at(iF));
      syncTimeEnergy[XtalHit_XtalNum->at(iF)]->Fill(XtalHit_Time->at(iF),XtalHit_Energy->at(iF));
    }
  }

  // Create histograms for time differences
  for(int i=0;i<54;i++){
    for(int j=0;j<54;j++){
      syncTimeDiff[i][j]->Fill(syncT[i]-syncT[j]);
      syncTimeDiffEnergy[i][j]->Fill(syncT[i]-syncT[j], 0.5*(syncE[i]+syncE[j]));
    } 
  } 

  // Collect single electron events only for the beam pulses
  bool isSingleElec = false;
  int islandNum = 0;
  for(size_t iC =0;iC<Cluster_EventNum->size();iC++){  
    if(Cluster_Energy->at(iC) < 3000 && Cluster_Energy->at(iC) > 1000 && Cluster_Time->at(iC)<2000 ) {
      isSingleElec = true; 
      islandNum = Cluster_IslandNum->at(iC); // store the islandNum here, loop through xtal with this islandNum
      cout<<"\tCluster Energy: "<<Cluster_Energy->at(iC)<<", Time: "<<Cluster_Time->at(iC)<<endl;
    }
  }

  // Only store the information if it is a single electron event
  if(isSingleElec) {

    for(size_t iFit =0;iFit<FitResult_EventNum->size();iFit++){  
      // continue if this fit result does not belong to the interested cluster
      if(FitResult_IslandNum->at(iFit) != islandNum) continue;

      if(FitResult_Time->at(iFit) > 2000 && FitResult_Time->at(iFit)<3000) {
        fittedBeamTime[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Time->at(iFit));
        fitT[FitResult_XtalNum->at(iFit)] = FitResult_Time->at(iFit); 
        fitE[FitResult_XtalNum->at(iFit)] = FitResult_Energy->at(iFit); 
        chi2->Fill(FitResult_Chi2->at(iFit));
        energyChi2->Fill(FitResult_Energy->at(iFit),FitResult_Chi2->at(iFit));
      }
    }

    for(size_t iF =0;iF<XtalHit_EventNum->size();iF++){  

      // continue if this xtal hit does not belong to the interested cluster
      if(XtalHit_IslandNum->at(iF) != islandNum) continue;

      if(XtalHit_Time->at(iF) > 1000 && XtalHit_Time->at(iF)<2000) {
        beamT[XtalHit_XtalNum->at(iF)] = XtalHit_Time->at(iF); 
        beamE[XtalHit_XtalNum->at(iF)] = XtalHit_Energy->at(iF); 
        beamTime[XtalHit_XtalNum->at(iF)]->Fill(XtalHit_Time->at(iF));
        beamEnergy[XtalHit_XtalNum->at(iF)]->Fill(XtalHit_Energy->at(iF));
        beamTimeEnergy[XtalHit_XtalNum->at(iF)]->Fill(XtalHit_Time->at(iF),XtalHit_Energy->at(iF));
      }
    }

    // create histograms for beam time differences
    for(int i=0;i<54;i++){
      for(int j=0;j<54;j++){
        if(beamT[i]!=0 && beamT[j]!=0 && (abs(beamT[i]-beamT[j])<2)  && beamE[i]>20 && beamE[j]>20) {
          beamTimeDiff[i][j]->Fill(beamT[i]-beamT[j]);
          beamTimeDiffEnergy[i][j]->Fill(beamT[i]-beamT[j], beamE[i]-beamE[j]);
        }
        if(fitT[i]!=0 && fitT[j]!=0 && (abs(fitT[i]-fitT[j])<10)  && fitE[i]>80 && fitE[j]>80) {
          fittedBeamTimeDiff[i][j]->Fill(fitT[i]-fitT[j]);
        }
      }
    }
  }

}

void GeneralAnalysis::finalize(string &filename){

  for(int i=0;i<54;i++){
    syncTime2D -> Fill (-i%9 - 0.5, i/9, syncTime[i]->GetMean());
    beamTime2D -> Fill (-i%9 - 0.5, i/9, beamTime[i]->GetMean());
    syncEnergy2D -> Fill (-i%9 - 0.5, i/9, syncEnergy[i]->GetMean());
    beamEnergy2D -> Fill (-i%9 - 0.5, i/9, beamEnergy[i]->GetMean());
  }

  cout << "\tfinalize(): " << filename<< endl;
  file->Write();
  file->Close();

}
