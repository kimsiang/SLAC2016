#include "WalkTimeAna.h"
#include <iostream>
#include <numeric> 

void WalkTimeAna::Loop(string filename){

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
void WalkTimeAna::initialize(string &filename){

    cout << "Initializing! Analyzing " << filename << endl;

    file = new TFile(Form("%s.root",filename.c_str()),"recreate");  

    for(int i=0;i<10;i++){
        laserTimes[i] = new TH1D(Form("laserTime%d",i),Form("laserTime%d",i),20000,0,2000);
    }

    for(int i=0;i<54;i++){
        dt[i] = new TH1D(Form("dt%d",i),Form("dt%d",i),200,50,70);
        syncE_dt[i] = new TH2D(Form("syncE_dt%d",i),Form("syncE_dt%d",i),100,50,70,100,0,2200);
    }
     
}

void WalkTimeAna::execute(){

    cout<<"EventNum: "<<EventNum<<endl;

    double localTime;

    for(int i=0;i<Italiano_UtcaSlotNum->size();i++){

        if(Italiano_Time->at(i)>2000) continue;

        int slotNum = Italiano_UtcaSlotNum->at(i)-5;
        int chNum = Italiano_ChannelNum->at(i);
        double time = Italiano_Time->at(i);
        cout<<"slotNum: "<<slotNum<<", chNum: "<<chNum<<", time: "<<time<<endl;
        laserTimes[slotNum*5+chNum]->Fill(time);
       if(slotNum*5+chNum==7) localTime = time;
    }

    double syncTime[54];
    double syncEnergy[54];

    for(int i=0;i<54;i++){
        syncTime[i] = FitResult_Time->at(i);
        syncEnergy[i] = FitResult_Energy->at(i)/4.;
        dt[i]->Fill(syncTime[i]-localTime);
        syncE_dt[i]->Fill(syncTime[i]-localTime, syncEnergy[i]);
    }

   cout<<"syncTime - localTime "<< syncTime[0]-localTime<<endl;


}

void WalkTimeAna::finalize(string &filename){
    file->Write();
    file->Close();

}
