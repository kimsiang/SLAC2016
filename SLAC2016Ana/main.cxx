#include <iostream>
#include <fstream>
#include <string>
#include "TString.h"
#include "SLAC2016Ana.h"
#include <TCanvas.h>
#include <cstdlib>

int main (int argc, char **argv) {

    // TCanvas *mydummycanvas = new TCanvas(); 

    string temp; // temp to store filename
    cout << "###############################################################" << endl;
    cout << "#####           Welcome to SLAC 2016 Analysis !           #####" << endl;
    cout << "###############################################################" << endl;
    cout << " " << endl;
    cout << "## Reading rootfiles to be analyzed........." << endl;
    cout << " " << endl;


    TChain *chain = new TChain ("slacAnalyzer/eventTree");

    string fname;
    ifstream inData (argv[1]); 

    while ( !inData.eof() ) {

	getline (inData, fname); //get contents of infile 
	cout << fname << endl; //output contents of infile   

	if(fname!=""){ // if this is not the last file

	    temp = fname;
	    int n = temp.rfind ("/"); // find the position in string of the last "/"
	    temp.erase (0, n + 1);	// erase substring in front of it including itself

	    chain->Add (TString(fname));
	}
    }

    SLAC2016Ana ana(chain);
    ana.Loop(temp);

    return 0;
}
