/*

   This is a simple parser to read output files from SimpleClient.

   To use it, simply compile it and run with 

   ./SCParser test.dat

   K.S. Khaw @ UW, 2016-04-29

*/
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <netdb.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include "debug.h"


// C++ include files
#include <iostream>
#include <iomanip>
#include <fstream>   
#include <sstream>
#include <string>
#include <bitset>
#include <math.h>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

// ROOT include files
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TFolder.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TSpectrum.h"
#include "TProfile.h"
#include "TROOT.h"

#include "FedEvent.hh"

// namespaces
using namespace std;

int main(int argc, char **argv)
{

    if ( argc < 2 ) {
	cout << "Error: Please insert filename!" << endl;
	return -1;
    }

    string inputFile = argv[1];

    cout << "Msg: Opening file " <<inputFile<<" ......"<<endl;
    ifstream *file = new ifstream(inputFile, ios::in|ios::binary);

    if ( !file->is_open() ) {
	cerr << "file open error:" << inputFile << endl;
    }


    //automatically change XXXX.dat to XXXX.root
    int file_len = strlen (argv[1]);
    inputFile.replace (file_len - 3, 3, "root");

    //create a new rootfile here
    TFile *treefile = new TFile ((char *) inputFile.c_str (), "recreate");
    cout << "Creating rootfile " << inputFile << " ......" << endl;

    vector<vector<short>> waveform;
    gROOT->ProcessLine("#include <vector>");
    waveform.resize(10);

    TTree *tree = new TTree ("myTree", "myTree");
    tree->Branch ("rider0_ch0_waveform", &waveform[0]);
    tree->Branch ("rider0_ch1_waveform", &waveform[1]);
    tree->Branch ("rider0_ch2_waveform", &waveform[2]);
    tree->Branch ("rider0_ch3_waveform", &waveform[3]);
    tree->Branch ("rider0_ch4_waveform", &waveform[4]);
    tree->Branch ("rider1_ch0_waveform", &waveform[5]);
    tree->Branch ("rider1_ch1_waveform", &waveform[6]);
    tree->Branch ("rider1_ch2_waveform", &waveform[7]);
    tree->Branch ("rider1_ch3_waveform", &waveform[8]);
    tree->Branch ("rider1_ch4_waveform", &waveform[9]);


    // Extract file length
    file->seekg (0, file->end);
    int length = file->tellg();
    cout<<"File length: "<<length<<endl;
    file->seekg (0, file->beg);

    dbprintf("hi");


    cout<<"----------------------------------------------"<<endl;

    FedAMC13::FedEvent fedevent;

    cout<<fedevent.PreParse((*file))<<endl;
    fedevent.ParseEvent();
   fedevent.Dump(3,true,0);

    for(size_t i=0; i<fedevent.GetnBlocks(); i++){
	
     //FedAMC13::FedAMC fedamc = fedevent.GetAMC(i);
//cout<<fedamc.AMCNo()<<endl;
    }

    //FedAMC13::FedAMC fedamc = fedevent.GetAMC(3);
    //cout<<fedamc.AMCNo()<<endl;

    // Close the input file
    file->close();

    // Write and close the root file
    treefile->Write ();
    treefile->Close ();

    return 0;
}

