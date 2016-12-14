#include <iostream>
#include <fstream>
#include <string>
#include "TString.h"
#include "CosmicRayAna.h"
#include "SLACRunLog.h"
#include <TCanvas.h>
#include <cstdlib>

int main (int argc, char **argv) {

    // TCanvas *mydummycanvas = new TCanvas(); 

    string temp1, temp2; // temp1 to store filename, temp2 to store run number
    cout << "###############################################################" << endl;
    cout << "#####           Welcome to SLAC 2016 Analysis !           #####" << endl;
    cout << "###############################################################" << endl;
    cout << " " << endl;
    cout << "## Reading rootfiles to be analyzed........." << endl;
    cout << " " << endl;


    SLACRunLog log;
    TChain *chain = new TChain ("slacAnalyzer/eventTree");

    string fname;
    ifstream inData (argv[1]); 

    while ( !inData.eof() ) {

	getline (inData, fname); //get contents of infile 
	cout << fname << endl; //output contents of infile   

	if(fname!=""){ // if this is not the last file

	    temp1 = fname;
	    int n = temp1.rfind ("/"); // find the position in string of the last "/"
	    temp1.erase (0, n + 1);	// erase substring in front of it including itself

	    temp2 = fname;
	    n = temp2.rfind ("run0"); // find the position in string of the last "/"
	    temp2.erase (0, n + 4);	// erase substring in front of it including itself

	    int run = stoi(temp2.erase (4, 8));	// erase substring in front of it including itself

	    if(log.GetRunLog(run)) {
		//    if(strcmp (log.quality,"T") == 0) continue;	
			    if(strcmp (log.quality,"N") == 0) continue;	
		    if(log.filterWheel> 5) continue;	
		cout<<"(log) runNum: "<<log.runNum<<", quality: "<<log.quality<<", FW: "<<log.filterWheel<<endl;
		chain->Add (TString(fname));
	    }
	}

    } 

    CosmicRayAna ana(chain);
    ana.Loop(temp2);

    return 0;
}
