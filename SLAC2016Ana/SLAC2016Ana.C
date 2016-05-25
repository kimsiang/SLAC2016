#include "SLAC2016Ana.h"
#include <iostream>

void SLAC2016Ana::Loop(string &filename){
    cout << "Loop()" << endl;

    initialize(filename);

    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();

    Int_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        clear();
        execute();

    }

    finalize(filename);

}
void SLAC2016Ana::initialize(string &filename){
    cout << "initialize()" << endl;

}

void SLAC2016Ana::execute(){
    cout << "execute()" << endl;

   for(const auto &length : (*Island_Length)){
//  cout<<length<<endl;
   }

}

void SLAC2016Ana::finalize(string &filename){

    cout << "finalize()" << endl;

}

void SLAC2016Ana::clear(){
    cout << "clear()" << endl;
}
