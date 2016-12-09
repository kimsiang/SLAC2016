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

    int nCandidateHits=0;
    int nNonLaser=0;

    vector<double> xtalE(0);
    vector<double> xtalNum(0);
    vector<double> xtalTime(0);
    vector<double> islandNum(0);
    vector<double> laserIslandNum(0);
    vector<double> nonLaserIslandNum(0);

    cout<<"#####################################################################"<<endl;
    cout<<"--> Run: "<<RunNum<<", EventNum: "<<EventNum+1<<endl;

    for(size_t i=0; i<Cluster_Energy->size();i++){
	if(Cluster_Time->at(i)>2000 && Cluster_Energy->at(i)<15000 && Cluster_Energy->at(i)> 50) {
	    nNonLaser++;
	    nonLaserIslandNum.push_back(Cluster_IslandNum->at(i));  
	    cout<<"Cluster IslandNum: "<<Cluster_IslandNum->at(i)<<", Time: "<<Cluster_Time->at(i)<<", Energy: "<<Cluster_Energy->at(i)<<endl;
	}

	else laserIslandNum.push_back(Cluster_IslandNum->at(i));  
    }

    if(nNonLaser>0){
	cout<<"--> Run: "<<RunNum<<", EventNum: "<<EventNum+1<<", nNonLaser: "<<nNonLaser<<endl;
    }

    else return; // return if there is no cosmic candidate based on cluster information


    for(size_t i=0; i<XtalHit_Energy->size();i++){

	bool isLaser=false;

	for(size_t j=0;j<laserIslandNum.size();j++){
	    if(XtalHit_IslandNum->at(i) == laserIslandNum[j]){
		isLaser=true;
	    }
	}

	if(isLaser==true) continue; // skip xtalhits from laser
	if(XtalHit_Time->at(i) < 2000 ) continue; // skip xtalhits from sync pulse and beam pulse

	if(XtalHit_Energy->at(i)<500 && XtalHit_Energy->at(i) > 20){
	    nCandidateHits++;
	    xtalE.push_back(XtalHit_Energy->at(i));
	    xtalNum.push_back(XtalHit_XtalNum->at(i));
	    xtalTime.push_back(XtalHit_Time->at(i));
	    islandNum.push_back(XtalHit_IslandNum->at(i));
	}

    }

    if(nCandidateHits>4 && nCandidateHits < 30){
	cout<<"--> Run: "<<RunNum<<", EventNum: "<<EventNum+1<<", nCandidateHits: "<<nCandidateHits<<endl;


	cout<<"IslandNums"<<" ";
	for(size_t i=0;i<islandNum.size();i++){
	    cout<<islandNum[i]<<" ";
	}
	cout<<endl;

	cout<<"XtalNums"<<" ";
	for(size_t i=0;i<xtalNum.size();i++){
	    cout<<xtalNum[i]<<" ";
	}
	cout<<endl;

	cout<<"XtalEnergies"<<" ";
	for(size_t i=0;i<xtalE.size();i++){
	    cout<<xtalE[i]<<" ";
	}
	cout<<endl;

	cout<<"XtalTimes"<<" ";
	for(size_t i=0;i<xtalTime.size();i++){
	    cout<<xtalTime[i]<<" ";
	}
	cout<<endl;

	vector<std::string> text(54);

	for(size_t i=0;i<54;i++){
	    text[i]=" ";
	}

	for(size_t i=0;i<xtalNum.size();i++){
	    text[xtalNum[i]]="X";
	}

	for(size_t i=0;i<54;i++){
	    cout<<"|"<<text[53-i];
	    if((53-i)%9==0) cout<<"|"<<endl;
	}

	std::cin.ignore(); // for inspection purpose
    }



}

void SLAC2016Ana::finalize(string &filename){

    cout << "\tfinalize(): " << filename<< endl;
    file->Write();
    file->Close();

}
