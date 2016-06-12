#include "SLAC2016Ana.h"
#include <iostream>

void SLAC2016Ana::Loop(string &filename){

    initialize();

    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();

    Int_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
	Long64_t ientry = LoadTree(jentry);
	if (ientry < 0) break;
	nb = fChain->GetEntry(jentry);   nbytes += nb;

	execute();

    }

    finalize();

}
void SLAC2016Ana::initialize(){

    cout << "initialize()" << endl;

    file_ = new TFile("test.root","recreate");
    energy_ = new TH1D("energy","energy",500,0,10000);
    //    energy_->SetCanExtend(kTRUE);


}

void SLAC2016Ana::execute(){

    cout << "--> execute(), EventNum: " << EventNum <<endl;


    for(size_t iC=0; iC<Cluster_EventNum->size();iC++){
	if(Cluster_Time->at(iC)>2000 && Cluster_Time->at(iC)<3000){
	    energy_->Fill(Cluster_Energy->at(iC));}
    }

}

void SLAC2016Ana::finalize(){

    cout << "\tfinalize()" << endl;

    file_->Write();
    file_->Close();

}
