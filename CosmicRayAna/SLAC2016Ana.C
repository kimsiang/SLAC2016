#include "SLAC2016Ana.h"
#include <iostream>
#include <numeric> 
#include "omp.h"

void SLAC2016Ana::Loop(string filename){

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
void SLAC2016Ana::initialize(string &filename){

    cout << "initialize()" << endl;

    file = new TFile("test.root","recreate");  

}

void SLAC2016Ana::execute(){

    int nCandidate=0;
    vector<double> xtalE(0);
    vector<double> xtalNum(0);
    vector<double> xtalTime(0);
    vector<double> islandNum(0);

    for(int i=0; i<XtalHit_Energy->size();i++){

	if(XtalHit_Time->at(i) < 2000 ) continue;

	if(XtalHit_Energy->at(i)<60 && XtalHit_Energy->at(i) > 20){
	    nCandidate++;
	    xtalE.push_back(XtalHit_Energy->at(i));
	    xtalNum.push_back(XtalHit_XtalNum->at(i));
	    xtalTime.push_back(XtalHit_Time->at(i));
	    islandNum.push_back(XtalHit_IslandNum->at(i));
	}

    }

    if(nCandidate>5 && nCandidate < 8){
	cout<<"#####################################################################"<<endl;
	cout<<"--> Run: "<<RunNum<<", EventNum: "<<EventNum<<", nCandidate: "<<nCandidate<<endl;

	cout<<"IslandNums"<<" ";
	for(int i=0;i<islandNum.size();i++){
	    cout<<islandNum[i]<<" ";
	}
	cout<<endl;

	cout<<"XtalNums"<<" ";
	for(int i=0;i<xtalNum.size();i++){
	    cout<<xtalNum[i]<<" ";
	}
	cout<<endl;

	cout<<"XtalEnergies"<<" ";
	for(int i=0;i<xtalE.size();i++){
	    cout<<xtalE[i]<<" ";
	}
	cout<<endl;

	cout<<"XtalTimes"<<" ";
	for(int i=0;i<xtalTime.size();i++){
	    cout<<xtalTime[i]<<" ";
	}
	cout<<endl;

	vector<std::string> text(54);

	for(int i=0;i<54;i++){
	    text[i]=" ";
	}

	for(int i=0;i<xtalNum.size();i++){
	    text[xtalNum[i]]="X";
	}

	for(int i=0;i<54;i++){
	    cout<<"|"<<text[53-i];
	    if((53-i)%9==0) cout<<"|"<<endl;
	}


	for(int i=0; i<XtalHit_Energy->size();i++){
	    cout<<"IslandNum: "<<XtalHit_IslandNum->at(i)<<", Beam XtalHit["<<XtalHit_XtalNum->at(i)<< "], Time: "<<XtalHit_Time->at(i)<<", Energy: "<<XtalHit_Energy->at(i)<<endl;
	}
    
	for(int i=0; i<FitResult_Energy->size();i++){
	    cout<<"IslandNum: "<<FitResult_IslandNum->at(i)<<", Beam FitResult["<<FitResult_XtalNum->at(i)<< "], Time: "<<FitResult_Time->at(i)<<", Energy: "<<FitResult_Energy->at(i)<<endl;
	}
    


}



}

void SLAC2016Ana::finalize(string &filename){

    cout << "\tfinalize(): " << filename<< endl;
    file->Write();
    file->Close();

}
