/*

   This is a simple parser to read output files from SimpleClient.

   To use it, simply compile it and run with 

   ./SCParser test.dat

   K.S. Khaw @ UW, 2016-04-29

*/

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


// namespaces
using namespace std;

struct CDF_HEADER
{
    bitset<4> Hx00;
    bitset<4> FOV;
    bitset<12> SourceID;
    bitset<12> BXID;
    bitset<24> LV1ID;
    bitset<4> EVTTY;
    bitset<4> Bit0x5;
};

struct CDF_TRAILER
{
    bitset<4> TR00;
    bitset<4> TTS;
    bitset<4> EVTSTAT;
    bitset<4> CFxx;
    bitset<16> CRC16;
    bitset<24> EVTLength;
    bitset<4> Empty;
    bitset<4> Bit0xA;
};

struct PAYLOAD_BLOCK_HEADER_1
{
    bitset<4> Bit0x0;
    bitset<32> OrN;
    bitset<16> Reserved;
    bitset<4> NAMC;
    bitset<4> Res;
    bitset<4> uFOV;
};

struct PAYLOAD_BLOCK_HEADER_2
{
    bitset<16> BoardID;
    bitset<4> AMCNo;
    bitset<8> BlockNo;
    bitset<4> Bit0x0;
    bitset<24> AMCSize;
    bitset<8> OLMSEPVC;
};

struct PAYLOAD_BLOCK_TRAILER
{
    bitset<12> BXID;
    bitset<8> LV1ID;
    bitset<8> BlockNo;
    bitset<4> Bit0x0;
    bitset<32> CRC32;
};

struct AMC13_HEADER_1
{
    bitset<20> DataLength;
    bitset<12> BXID;
    bitset<24> LV1ID;
    bitset<4> AMCNo;
    bitset<4> Bit0x0;
};

struct AMC13_HEADER_2
{
    bitset<16> BoardID;
    bitset<32> OrN;
    bitset<16> User;
};

struct AMC13_TRAILER
{
    bitset<20> DataLength;
    bitset<4> Bit0x0;
    bitset<8> LV1ID;
    bitset<32> CRC32;
};

struct RIDER_TRAILER
{
    bitset<64> Checksum1;
    bitset<64> Checksum2;
    bitset<32> DataTransTime;
    bitset<32> DataIntCheck;
};

struct RIDER_CH_HEADER_1
{
    bitset<12> DDR3;
    bitset<12> WFCount;
    bitset<22> WFGap;
    bitset<16> CHTag;
    bitset<2> Bit01;
};

struct RIDER_CH_HEADER_2
{
    bitset<24> TrigNum;
    bitset<3> FT;
    bitset<23> WFLength;
    bitset<14> DDR3;
};

struct RIDER_WF_HEADER_1
{
    bitset<23> WFLength;
    bitset<3> FT;
    bitset<26> DDR3;
    bitset<12> WFCount;
};

struct RIDER_WF_HEADER_2
{
    bitset<12> WFIndex;
    bitset<22> WFGap;
    bitset<16> CHTag;
    bitset<12> Bit0x000;
    bitset<2> Bit01;
};

// forward declaration
void Check_CDF_Header(ifstream &file);
uint16_t Check_Payload_Block_Header(ifstream &file);
void Check_AMC13_Header(ifstream &file);
uint16_t Check_Rider_CH_Header(ifstream &file);
uint16_t Check_Rider_WF_Header(ifstream &file);
vector<short> Extract_ADC_Data(ifstream &file, uint16_t n_sample);
void Check_Rider_CH_Trailer(ifstream &file);
void Check_AMC13_Trailer(ifstream &file);
void Check_Payload_Block_Trailer(ifstream &file);
void Check_CDF_Trailer(ifstream &file);

int main(int argc, char **argv)
{

    if ( argc < 2 ) {
	cout << "Error: Please insert filename!" << endl;
	return -1;
    }

    string inputFile = argv[1];

    cout << "Msg: Opening file " <<inputFile<<" ......"<<endl;
    ifstream file (inputFile, ios::in|ios::binary);

    if ( !file.is_open() ) {
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
    file.seekg (0, file.end);
    int length = file.tellg();
    cout<<"File length: "<<length<<endl;
    file.seekg (0, file.beg);

    cout<<"----------------------------------------------"<<endl;

    // Read the file until the last position
    while(file.tellg()<length){

	// 1. Check the CDF Header
	Check_CDF_Header(file);

	// 2. Check the Payload Block Header and decide the No. of AMC
	uint16_t n_amc = Check_Payload_Block_Header(file);

	for(int i=0;i<n_amc;i++) {

	    cout<<"\t\t"<<"###### Checking AMC"<<i+1<<" Header ######"<<endl;
	    cout<<""<<endl;

	    // 3. Check the AMC13 Header
	    Check_AMC13_Header(file);

	    for(int j=0;j<5;j++){

		cout<<"\t\t\t"<<"###### Checking Rider"<<j<<" Header ######"<<endl;
		cout<<""<<endl;

		// 4. Check the Rider Channel Header
		uint16_t n_sample = Check_Rider_CH_Header(file);

		// 5. Check the Rider Waveform Header
		Check_Rider_WF_Header(file);

		// 6. Extract ADC Data
		waveform[i*5+j] = Extract_ADC_Data(file, n_sample);

		// 7. Check the Rider Channel Trailer
		Check_Rider_CH_Trailer(file);
	    }

	    // 8. Check the AMC13 Trailer
	    Check_AMC13_Trailer(file);
	}

	// 9. Check the Payload Block Trailer
	Check_Payload_Block_Trailer(file);

	// 10. Check the CDF Trailer
	Check_CDF_Trailer(file);
        
        tree->Fill();

	cout<<"----> Current read position: "<<file.tellg()<<"/"<<length<<"("<<(float)(100.*file.tellg()/length)<<"%)"<<endl;
	cout<<"----------------------------------------------"<<endl;

    }

    // Close the input file
    file.close();

    // Write and close the root file
    treefile->Write ();
    treefile->Close ();

    return 0;
}

void Check_CDF_Header(ifstream &file){

    CDF_HEADER cdf_header;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    cdf_header.Hx00 = temp;
    cdf_header.FOV = (temp>>4);
    cdf_header.SourceID = (temp>>8);
    cdf_header.BXID = (temp>>20);
    cdf_header.LV1ID = (temp>>32);
    cdf_header.EVTTY = (temp>>56);
    cdf_header.Bit0x5 = (temp>>60);

    cout<<"###### Checking CDF Header ######"<<endl;
    cout<<setw(12) << right <<"Hx$$: "<< cdf_header.Hx00 << endl;
    cout<<setw(12) << right <<"FOV: "<< cdf_header.FOV << endl;
    cout<<setw(12) << right <<"SourceID: "<< cdf_header.SourceID << endl;
    cout<<setw(12) << right <<"BXID: "<< cdf_header.BXID << endl;
    cout<<setw(12) << right <<"LV1ID: "<< cdf_header.LV1ID << endl;
    cout<<setw(12) << right <<"EVTTY: "<< cdf_header.EVTTY << endl;
    cout<<setw(12) << right <<"0x5: "<< cdf_header.Bit0x5 << endl;
    cout<<"----------------------------------------------"<<endl;

}

void Check_CDF_Trailer(ifstream &file){

    CDF_TRAILER cdf_trailer;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    cdf_trailer.TR00 = temp;
    cdf_trailer.TTS = (temp>>4);
    cdf_trailer.EVTSTAT = (temp>>8);
    cdf_trailer.CFxx = (temp>>12);
    cdf_trailer.CRC16 = (temp>>16);
    cdf_trailer.EVTLength = (temp>>32);
    cdf_trailer.Empty = (temp>>56);
    cdf_trailer.Bit0xA = (temp>>60);

    cout<<"###### Checking CDF Trailer ######"<<endl;
    cout<<setw(12) << right <<"TR$$: "<< cdf_trailer.TR00 << endl;
    cout<<setw(12) << right <<"TTS: "<< cdf_trailer.TTS << endl;
    cout<<setw(12) << right <<"EVTSTAT: "<< cdf_trailer.EVTSTAT << endl;
    cout<<setw(12) << right <<"CFxx: "<< cdf_trailer.CFxx << endl;
    cout<<setw(12) << right <<"CRC(16): "<< cdf_trailer.CRC16 << endl;
    cout<<setw(12) << right <<"EVTLength: "<< cdf_trailer.EVTLength << endl;
    cout<<setw(12) << right <<"Empty: "<< cdf_trailer.Empty << endl;
    cout<<setw(12) << right <<"0xA: "<< cdf_trailer.Bit0xA << endl;
    cout<<"----------------------------------------------"<<endl;

}

uint16_t Check_Payload_Block_Header(ifstream &file){

    PAYLOAD_BLOCK_HEADER_1 payload_h1;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    payload_h1.Bit0x0 = temp;
    payload_h1.OrN = (temp>>4);
    payload_h1.Reserved = (temp>>36);
    payload_h1.NAMC = (temp>>52);
    uint8_t n_amc = payload_h1.NAMC.to_ulong(); 
    payload_h1.Res = (temp>>56);
    payload_h1.uFOV = (temp>>60);


    cout<<"\t###### Checking Payload Block Header 1 ######"<<endl;
    cout<<"\t"<<setw(12) << right <<"Bit0x0: "<< payload_h1.Bit0x0 << endl;
    cout<<"\t"<<setw(12) << right <<"OrN: "<< payload_h1.OrN << endl;
    cout<<"\t"<<setw(12) << right <<"Reserved: "<< payload_h1.Reserved << endl;
    cout<<"\t"<<setw(12) << right <<"NAMC: "<< payload_h1.NAMC << endl;
    cout<<"\t"<<setw(12) << right <<"Res: "<< payload_h1.Res << endl;
    cout<<"\t"<<setw(12) << right <<"uFOV: "<< payload_h1.uFOV << endl;


    for(unsigned int i=0;i<n_amc;i++){

	PAYLOAD_BLOCK_HEADER_2 payload_h2;
	file.read((char*)&temp, sizeof(temp)); 

	payload_h2.BoardID = temp;
	payload_h2.AMCNo = (temp>>16);
	payload_h2.BlockNo = (temp>>20);
	payload_h2.Bit0x0 = (temp>>28);
	payload_h2.AMCSize = (temp>>32);
	payload_h2.OLMSEPVC = (temp>>56);

	cout<<""<<endl;
	cout<<"\t###### Checking Payload Block Header AMC"<<i+1<<" ######"<<endl;
	cout<<"\t"<<setw(12) << right <<"BoardID: "<< payload_h2.BoardID << endl;
	cout<<"\t"<<setw(12) << right <<"AMCNo: "<< payload_h2.AMCNo << endl;
	cout<<"\t"<<setw(12) << right <<"BlockNo: "<< payload_h2.BlockNo << endl;
	cout<<"\t"<<setw(12) << right <<"Bit0x0: "<< payload_h2.Bit0x0 << endl;
	cout<<"\t"<<setw(12) << right <<"AMCSize: "<< payload_h2.AMCSize << endl;
	cout<<"\t"<<setw(12) << right <<"OLMSEPVC: "<< payload_h2.OLMSEPVC << endl;
    }

    cout<<"\t"<<"----------------------------------------------"<<endl;

    return n_amc;
}

void Check_Payload_Block_Trailer(ifstream &file){

    bitset<12> BXID;
    bitset<8> LV1ID;
    bitset<8> BlockNo;
    bitset<4> Bit0x0;
    bitset<32> CRC32;

    PAYLOAD_BLOCK_TRAILER payload_t;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    payload_t.BXID = temp;
    payload_t.LV1ID = (temp>>12);
    payload_t.BlockNo = (temp>>20);
    payload_t.Bit0x0 = (temp>>28);
    payload_t.CRC32 = (temp>>32);


    cout<<"\t###### Checking Payload Block Trailer ######"<<endl;
    cout<<"\t"<<setw(12) << right <<"BXID: "<< payload_t.BXID << endl;
    cout<<"\t"<<setw(12) << right <<"LV1ID: "<< payload_t.LV1ID << endl;
    cout<<"\t"<<setw(12) << right <<"BlockNo: "<< payload_t.BlockNo << endl;
    cout<<"\t"<<setw(12) << right <<"Bit0x0: "<< payload_t.Bit0x0 << endl;
    cout<<"\t"<<setw(12) << right <<"CRC32: "<< payload_t.CRC32 << endl;

    cout<<"\t"<<"----------------------------------------------"<<endl;
}



void Check_AMC13_Header(ifstream &file){

    AMC13_HEADER_1 amc13_h1;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    amc13_h1.DataLength = temp;
    amc13_h1.BXID = (temp>>20);
    amc13_h1.LV1ID = (temp>>32);
    amc13_h1.AMCNo = (temp>>56);
    amc13_h1.Bit0x0 = (temp>>60);

    cout<<"\t\t"<<"###### Checking AMC13 Header 1 ######"<<endl;
    cout<<"\t\t"<<setw(12) << right <<"DataLength: "<< amc13_h1.DataLength << endl;
    cout<<"\t\t"<<setw(12) << right <<"BXID: "<< amc13_h1.BXID << endl;
    cout<<"\t\t"<<setw(12) << right <<"LV1ID: "<< amc13_h1.LV1ID << endl;
    cout<<"\t\t"<<setw(12) << right <<"AMCNo: "<< amc13_h1.AMCNo << endl;
    cout<<"\t\t"<<setw(12) << right <<"Bit0x0: "<< amc13_h1.Bit0x0 << endl;

    AMC13_HEADER_2 amc13_h2;
    file.read((char*)&temp, sizeof(temp)); 

    amc13_h2.BoardID = temp;
    amc13_h2.OrN = (temp>>16);
    amc13_h2.User = (temp>>48);

    cout<<""<<endl;
    cout<<"\t\t"<<"###### Checking AMC13 Header 2 ######"<<endl;
    cout<<"\t\t"<<setw(12) << right <<"BoardID: "<< amc13_h2.BoardID << endl;
    cout<<"\t\t"<<setw(12) << right <<"OrN: "<< amc13_h2.OrN<< endl;
    cout<<"\t\t"<<setw(12) << right <<"User: "<< amc13_h2.User << endl;
    cout<<"\t\t"<<"----------------------------------------------"<<endl;

}

void Check_AMC13_Trailer(ifstream &file){

    AMC13_TRAILER amc13_t;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    amc13_t.DataLength = temp;
    amc13_t.Bit0x0 = (temp>>20);
    amc13_t.LV1ID = (temp>>24);
    amc13_t.CRC32 = (temp>>32);

    cout<<"\t\t"<<"###### Checking AMC13 Trailer ######"<<endl;
    cout<<"\t\t"<<setw(12) << right <<"DataLength: "<< amc13_t.DataLength << endl;
    cout<<"\t\t"<<setw(12) << right <<"Bit0x0: "<< amc13_t.Bit0x0 << endl;
    cout<<"\t\t"<<setw(12) << right <<"LV1ID: "<< amc13_t.LV1ID << endl;
    cout<<"\t\t"<<setw(12) << right <<"CRC(32): "<< amc13_t.CRC32 << endl;

    cout<<"\t\t"<<"----------------------------------------------"<<endl;
}

uint16_t Check_Rider_CH_Header(ifstream &file){

    RIDER_CH_HEADER_1 rider_ch_h1;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    rider_ch_h1.DDR3 = temp;
    rider_ch_h1.WFCount = (temp>>12);
    rider_ch_h1.WFGap = (temp>>24);
    rider_ch_h1.CHTag = (temp>>46);
    rider_ch_h1.Bit01 = (temp>>62);

    cout<<"\t\t\t"<<"###### Checking Rider CH Header 1 ######"<<endl;
    cout<<"\t\t\t"<<setw(12) << right <<"DDR3: "<< rider_ch_h1.DDR3 << endl;
    cout<<"\t\t\t"<<setw(12) << right <<"WFCount: "      << rider_ch_h1.WFCount << endl;
    cout<<"\t\t\t"<<setw(12) << right <<"WFGap: "    << rider_ch_h1.WFGap << endl;
    cout<<"\t\t\t"<<setw(12) << right <<"CHTag: " << rider_ch_h1.CHTag << endl;
    cout<<"\t\t\t"<<setw(12) << right <<"Bit01: " << rider_ch_h1.Bit01 << endl;


    RIDER_CH_HEADER_2 rider_ch_h2;
    file.read((char*)&temp, sizeof(temp)); 

    rider_ch_h2.TrigNum = temp;
    rider_ch_h2.FT = (temp>>24);
    rider_ch_h2.WFLength = (temp>>27);
    uint16_t n_sample = rider_ch_h2.WFLength.to_ulong(); 
    rider_ch_h2.DDR3 = (temp>>50);


    cout<<""<<endl;
    cout<<"\t\t\t"<<"###### Checking Rider CH Header 2 ######"<<endl;
    cout<<"\t\t\t"<<setw(12) << right <<"TrigNum: "  << rider_ch_h2.TrigNum << endl;
    cout<<"\t\t\t"<<setw(12) << right <<"FT: "       << rider_ch_h2.FT << endl;
    cout<<"\t\t\t"<<setw(12) << right <<"WFLength: " << rider_ch_h2.WFLength << " ("<<n_sample<<")"<<endl;
    cout<<"\t\t\t"<<setw(12) << right <<"DDR3: "     << rider_ch_h2.DDR3 << endl;
    cout<<"\t\t\t"<<"----------------------------------------------"<<endl;

    return n_sample;
}

uint16_t Check_Rider_WF_Header(ifstream &file){

    cout<<"\t\t\t\t"<<"###### Checking Rider WF Header 1 ######"<<endl;

    RIDER_WF_HEADER_1 rider_wf_h1;
    uint64_t temp;
    file.read((char*)&temp, sizeof(temp)); 

    rider_wf_h1.WFLength = temp;
    uint16_t n_sample = rider_wf_h1.WFLength.to_ulong(); 
    rider_wf_h1.FT = (temp>>23);
    rider_wf_h1.DDR3 = (temp>>26);
    rider_wf_h1.WFCount = (temp>>52);


    cout<<"\t\t\t\t"<<setw(12) << right <<"WFLength: "<< rider_wf_h1.WFLength <<" ("<< n_sample<<")"<<endl;
    cout<<"\t\t\t\t"<<setw(12) << right <<"FT: "<< rider_wf_h1.FT << endl;
    cout<<"\t\t\t\t"<<setw(12) << right <<"DDR3: "<< rider_wf_h1.DDR3 << endl;
    cout<<"\t\t\t\t"<<setw(12) << right <<"WFCount: "<< rider_wf_h1.WFCount << endl;

    cout<<""<<endl;
    cout<<"\t\t\t\t"<<"###### Checking Rider WF Header 2 ######"<<endl;

    RIDER_WF_HEADER_2 rider_wf_h2;
    file.read((char*)&temp , sizeof(temp)); 

    rider_wf_h2.WFIndex = temp;
    rider_wf_h2.WFGap = (temp>>12);
    rider_wf_h2.CHTag = (temp>>34);
    rider_wf_h2.Bit0x000 = (temp>>50);
    rider_wf_h2.Bit01 = (temp>>62);


    cout<<"\t\t\t\t"<<setw(12) << right <<"WFIndex: "<< rider_wf_h2.WFIndex << endl;
    cout<<"\t\t\t\t"<<setw(12) << right <<"WFGap: "<< rider_wf_h2.WFGap << endl;
    cout<<"\t\t\t\t"<<setw(12) << right <<"CHTag: "<< rider_wf_h2.CHTag << endl;
    cout<<"\t\t\t\t"<<setw(12) << right <<"0x000: "<< rider_wf_h2.Bit0x000 << endl;
    cout<<"\t\t\t\t"<<setw(12) << right <<"01: "<< rider_wf_h2.Bit01 << endl;
    cout<<"\t\t\t\t"<<"----------------------------------------------"<<endl;

    return n_sample;
}

void Check_Rider_CH_Trailer(ifstream &file){

    RIDER_TRAILER rider_trailer;
    uint64_t temp;

    file.read((char*)&temp , sizeof(temp)); 
    rider_trailer.Checksum1 = temp;

    file.read((char*)&temp , sizeof(temp)); 
    rider_trailer.Checksum2 = temp;

    file.read((char*)&temp , sizeof(temp)); 
    rider_trailer.DataTransTime = temp;
    rider_trailer.DataIntCheck = (temp>>32);

    cout<<"\t\t\t"<<"###### Checking Rider CH Trailer ######"<<endl;
    cout<<"\t\t\t"<<setw(15) << right <<"Checksum1: "<< rider_trailer.Checksum1 << endl;
    cout<<"\t\t\t"<<setw(15) << right <<"Checksum2: "<< rider_trailer.Checksum2 << endl;
    cout<<"\t\t\t"<<setw(15) << right <<"DataTransTime: "<< rider_trailer.DataTransTime << endl;
    cout<<"\t\t\t"<<setw(15) << right <<"DataIntCheck: " << hex <<rider_trailer.DataIntCheck.to_ulong() << dec << endl;
    cout<<"\t\t\t"<<"----------------------------------------------"<<endl;

}

vector<short> Extract_ADC_Data(ifstream &file, uint16_t n_sample){

    cout<<""<<endl;
    cout<<"\t\t\t\t"<<"###### Extracting Rider ADC Data ######"<<endl;
    vector<short> trace(n_sample);
    file.read((char*)&trace[0], 2*n_sample);

    cout<<"\t\t\t\t"<<"###### Loop through "<<n_sample<<" traces ######"<< endl;

    //   for(unsigned int i=0;i<trace.size();i+=4){
    //	cout<<"\t\t\t\t"<<"#"<<setw(5) << left <<i<<"= "<<trace[i]<<", #"<<setw(5) << left <<i+1<<"= "<<trace[i+1]
    //	    <<", #"<<setw(5) << left <<i+2<<"= "<<trace[i+2]<<", #"<<setw(5) << left <<i+3<<"= "<<trace[i+3]<<","<<endl;
    //   }

    cout<<"\t\t\t\t"<<"----------------------------------------------"<<endl;

   return trace;

}
