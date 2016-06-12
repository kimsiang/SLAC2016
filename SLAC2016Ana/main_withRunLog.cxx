#include <iostream>
#include <fstream>
#include <string>
#include "TString.h"
#include "SLAC2016Ana.h"
#include <TCanvas.h>
#include "SLACRunLog.h"
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

    unsigned int size=0; 
    string line;
    ifstream inData (argv[1]); 

    while ( !inData.eof() )
    {
	getline (inData, line); //get contents of infile 
	cout << line << endl; //output contents of infile   
	size++;
    } 

    // get run log here
    SLACRunLog log;

    for(unsigned int j=1;j<size;j++){
	TChain *chain = new TChain ("slacAnalyzer/eventTree");
	string fname;
	if (argc > 1)
	{
	    ifstream fin (argv[1]);
	    if (!fin.is_open ())
	    {
		cerr << "file open error:" << argv[1] << endl;
		exit (1);
	    }
	    unsigned int i = 0;
	    while (fin >> fname)
	    {
		i++;
		if(i==j){
		    temp1 = fname;
		    temp2 = fname;
		    int n = temp1.rfind ("/"); // find the position in string of the last "/"
		    temp1.erase (0, n + 1);	// erase substring in front of it including itself
		    n = temp2.rfind ("run0"); // find the position in string of the last "/"
		    temp2.erase (0, n + 4);	// erase substring in front of it including itself
		    int run = stoi(temp2.erase (4, 8));	// erase substring in front of it including itself

		    if(log.GetRunLog(run)) {
			cout<<"(log) runNum: "<<log.runNum<<", quality: "<<log.quality<<endl;
			chain->Add (TString(fname));
		    }
		    else continue;
		}
	    }

	    fin.close();   
	}

	else
	{
	    cerr << "no input file" << endl;
	    exit (1);
	}

	cout <<"###################################################################"<< endl;
	cout <<"##    Current file is : "<< temp1 << "   ##"<<endl; // print the input rootfile name
	cout <<"###################################################################"<< endl;

//SLAC2016Ana ana(chain);
//	ana.Loop(temp1);
    }

    return 0;
}
