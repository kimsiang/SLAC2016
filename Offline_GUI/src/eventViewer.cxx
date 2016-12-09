#include "../include/IslandArtRecord.hh"
#include "../include/FitResultArtRecord.hh"

#include "assert.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TFile.h"
#include "TSpline.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TPaveText.h"
#include "TString.h"
#include "TAxis.h"
#include "TF1.h"
#include "TTree.h"
#include "TColor.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <numeric>
#include <memory>

using namespace std;

bool drawing = true;

void displayIslands(const gm2calo::IslandArtRecord& islandRecord);
void displayResults(const gm2calo::FitResultArtRecordCollection& frr,
	const gm2calo::IslandArtRecord& islandRecord,
	TSpline3* tSpline);

typedef struct {
    UInt_t eventNum;
    UInt_t caloNum;
    UInt_t xtalNum;
    UInt_t islandNum;
    UInt_t length;
    UInt_t time;
    vector<Short_t> *trace;
} ISLAND;

typedef struct {
    vector<UInt_t> *eventNum;
    vector<UInt_t> *caloNum;
    vector<UInt_t> *xtalNum;
    vector<UInt_t> *islandNum;
    vector<Double_t> *pedestal;
    vector<Double_t> *energy;
    vector<Double_t> *time;
    vector<Double_t> *chi2;
    vector<Double_t> *pe;
} FIT_RESULT;

int main(int argc, char const* argv[]) {
    new TApplication("app", 0, nullptr);

    if (argc < 2) {
	cout << "need input file" << endl;
	exit(0);
    }

    vector<std::unique_ptr<TSpline3>> splines;
    for (int i = 0; i < 54; ++i) {
	unique_ptr<TFile> splineF(new TFile(Form("../SLACTemplates/laserTemplates/laserTemplateFile%i.root", i)));
	splines.emplace_back((TSpline3*)splineF->Get("masterSpline"));
    }

    vector<std::unique_ptr<TSpline3>> beamSplines;
    for (int i = 0; i < 54; ++i) {
	unique_ptr<TFile> splineF(new TFile(Form("../SLACTemplates/beamTemplates/beamTemplateFile%i.root", i)));
	beamSplines.emplace_back((TSpline3*)splineF->Get("masterSpline"));
    }

    TFile infile(argv[1]);
    unique_ptr<TTree> tIsland((TTree*)infile.Get("slacAnalyzer/islandTree"));
    unique_ptr<TTree> tFitresult((TTree*)infile.Get("slacAnalyzer/eventTree"));

    vector<gm2calo::IslandArtRecord> islands;
    vector<gm2calo::FitResultArtRecord> fitresultcol;

    ISLAND island = {0,0,0,0,0,0,0};  
    FIT_RESULT fitresult = {0,0,0,0,0,0,0,0,0};  

    tIsland->SetBranchAddress("EventNum", &island.eventNum);
    tIsland->SetBranchAddress("CaloNum", &island.caloNum);
    tIsland->SetBranchAddress("XtalNum", &island.xtalNum);
    tIsland->SetBranchAddress("IslandNum", &island.islandNum);
    tIsland->SetBranchAddress("Length", &island.length);
    tIsland->SetBranchAddress("Time", &island.time);
    tIsland->SetBranchAddress("Trace", &island.trace);

    tFitresult->SetBranchAddress("FitResult_EventNum", &fitresult.eventNum);
    tFitresult->SetBranchAddress("FitResult_CaloNum", &fitresult.caloNum);
    tFitresult->SetBranchAddress("FitResult_XtalNum", &fitresult.xtalNum);
    tFitresult->SetBranchAddress("FitResult_IslandNum", &fitresult.islandNum);
    tFitresult->SetBranchAddress("FitResult_Pedestal", &fitresult.pedestal);
    tFitresult->SetBranchAddress("FitResult_Energy", &fitresult.energy);
    tFitresult->SetBranchAddress("FitResult_Time", &fitresult.time);
    tFitresult->SetBranchAddress("FitResult_Chi2", &fitresult.chi2);
    tFitresult->SetBranchAddress("XtalHit_Energy", &fitresult.pe);

    vector<double>* clusterE = 0;
    tFitresult->SetBranchAddress("Cluster_Energy", &clusterE);
    vector<double>* clusterIslandNum = 0;
    tFitresult->SetBranchAddress("Cluster_IslandNum", &clusterIslandNum);

    for (int i = 0; i < tIsland->GetEntries(); i++) {

	tIsland->GetEntry(i);

	if(island.eventNum!= atoi(argv[2])) continue;
	//if(island.xtalNum%9!=3 ) continue;
	if(island.islandNum!= atoi(argv[3]) ) continue;

	gm2calo::IslandArtRecord islandRecord;
	islandRecord.fillNum = island.eventNum;
	islandRecord.caloNum = island.caloNum;
	islandRecord.xtalNum = island.xtalNum;
	islandRecord.islandNum = island.islandNum;
	islandRecord.firstSampleNum = island.time;
	islandRecord.trace = *(island.trace);
	islands.push_back(islandRecord);

	tFitresult->GetEntry(island.eventNum-1);

	for(size_t j=0; j<fitresult.eventNum->size(); j++){


	    if(island.xtalNum == (*fitresult.xtalNum)[j] && 
		    island.islandNum == (*fitresult.islandNum)[j]){

		gm2calo::FitResultArtRecord fitresultRecord;
		fitresultRecord.fillNum = (*fitresult.eventNum)[j];
		fitresultRecord.caloNum = (*fitresult.caloNum)[j];
		fitresultRecord.xtalNum = (*fitresult.xtalNum)[j];
		fitresultRecord.islandNum = (*fitresult.islandNum)[j];
		fitresultRecord.pedestal = (*fitresult.pedestal)[j];
		fitresultRecord.energy = (*fitresult.energy)[j];
		fitresultRecord.time = (*fitresult.time)[j];
		fitresultRecord.chi2 = (*fitresult.chi2)[j];
		fitresultcol.push_back(fitresultRecord);    

		//std::cout<<"pe: "<<fitresult.pe->at(0)<<std::endl;
	    }

	}

	std::vector<double> theseClusterE;
	for (unsigned int k = 0; k < clusterE->size(); ++k) {
	    if (clusterIslandNum->at(k) == islandRecord.islandNum) {
		theseClusterE.push_back(clusterE->at(k));
	    }
	}

	TSpline3* thisSpline = theseClusterE[0] < 10000
	    ? beamSplines[islandRecord.xtalNum].get()
	    : splines[islandRecord.xtalNum].get();


	//displayIslands(islandRecord);
	displayResults(fitresultcol, islandRecord, thisSpline);
	fitresultcol.resize(0);

	//return 0;
    }
}

void displayIslands(const gm2calo::IslandArtRecord& islandRecord){

    const auto& trace = islandRecord.trace;
    std::vector<int> sampleTimes(trace.size());
    std::iota(sampleTimes.begin(), sampleTimes.end(),
	    islandRecord.firstSampleNum);

    std::string title = "event " + std::to_string(islandRecord.fillNum) + " calo " + std::to_string(islandRecord.caloNum) +
	" xtal " + std::to_string(islandRecord.xtalNum) +
	" island " + std::to_string(islandRecord.islandNum);

    TFile lastCanvas("lastCanvas.root", "recreate");
    // make plot
    std::unique_ptr<TCanvas> c(
	    new TCanvas("c", (title + "_canvas").c_str(), 800, 600));

    std::unique_ptr<TGraph> g(new TGraph(0));
    g->SetTitle(title.c_str());
    for (size_t i = 0; i < trace.size(); ++i) {
	g->SetPoint(g->GetN(), sampleTimes[i], trace[i]);
    }

    g->SetMarkerStyle(20);
    g->Draw("ap");
    g->GetXaxis()->SetRangeUser(sampleTimes[0], sampleTimes.back());
    g->GetXaxis()->SetTitle("sample number");
    g->GetYaxis()->SetTitle("ADC counts");
    g->GetYaxis()->SetTitleOffset(1.5);

    double yMin = g->GetYaxis()->GetXmin();
    double yMax = g->GetYaxis()->GetXmax();
    double xRangeMin = sampleTimes[0];
    double xRangeMax = sampleTimes.back();
    std::unique_ptr<TPaveText> txtbox(new TPaveText(
		xRangeMin + (xRangeMax - xRangeMin) * 0.6, yMin + (yMax - yMin) * 0.9,
		xRangeMin + (xRangeMax - xRangeMin) * 0.9, yMin + (yMax - yMin) * 0.65));
    txtbox->SetFillColor(kWhite);

    c->Modified();
    c->Update();
    c->Draw();
    gSystem->ProcessEvents();

    c->Write();
    lastCanvas.Write();

    std::cout << title << " displayed. Any key to move on" << std::endl;
    std::cin.ignore();

}


void displayResults(const gm2calo::FitResultArtRecordCollection& frr,
	const gm2calo::IslandArtRecord& islandRecord,
	TSpline3* tSpline) {
    assert(frr.size() > 0);

    const auto& trace = islandRecord.trace;
    std::vector<int> sampleTimes(trace.size());
    std::iota(sampleTimes.begin(), sampleTimes.end(),
	    islandRecord.firstSampleNum);

    std::string title = "run 2159 event " + std::to_string(islandRecord.fillNum) + //" calo " + std::to_string(islandRecord.caloNum) +
	" xtal " + std::to_string(islandRecord.xtalNum) +
	" island " + std::to_string(islandRecord.islandNum);

    std::string filename =  "event" + std::to_string(islandRecord.fillNum) + //"_calo" + std::to_string(islandRecord.caloNum) +
	"_xtal" + std::to_string(islandRecord.xtalNum) +
	"_island" + std::to_string(islandRecord.islandNum) + ".root";


    TFile lastCanvas("lastCanvas.root", "recreate");

    const int nPulses = frr.size();

    // print to terminal
    std::cout << title << std::endl;
    for (int i = 0; i < nPulses; ++i) {
	std::cout << "t" << i + 1 << ": " << frr[i].time << std::endl;
	std::cout << "scale" << i + 1 << ": " << frr[i].energy << std::endl;
    }
    std::cout << "pedestal: " << frr[0].pedestal << std::endl;
    std::cout << "chi2: " << frr[0].chi2 << std::endl;
    std::cout << std::endl;
    std::cout << "done printing" << std::endl;

    // make plot
    std::unique_ptr<TCanvas> c(
	    new TCanvas("c", (title + "_canvas").c_str(), 800, 600));

    std::unique_ptr<TGraph> g(new TGraph(0));
    g->SetTitle(title.c_str());
    for (size_t i = 0; i < trace.size(); ++i) {
	g->SetPoint(g->GetN(), sampleTimes[i], trace[i]);
    }

    // room for up to three pulses
    double xMin = tSpline->GetXmin();
    double xMax = tSpline->GetXmax();
    auto templateFunction = [&](double* x, double* p) {
	double returnValue = p[6];
	for (int i = 0; (i < nPulses) && (i < 3); ++i) {
	    if ((x[0] - p[2 * i] > xMin) && (x[0] - p[2 * i] < xMax)) {
		returnValue += p[1 + 2 * i] * tSpline->Eval(x[0] - p[2 * i]);
	    }
	}
	return returnValue;
    };

    std::unique_ptr<TF1> func(new TF1("fitFunc", templateFunction, sampleTimes[0],
		sampleTimes.back(), 7));
    func->SetLineColor(kBlack);
    func->SetParameters(std::vector<double>(7, 0).data());

    g->SetMarkerStyle(20);
    g->Draw("ap");
    g->GetXaxis()->SetRangeUser(sampleTimes[0], sampleTimes.back());
    g->GetXaxis()->SetTitle("sample number");
    g->GetYaxis()->SetTitle("ADC counts");
    g->GetYaxis()->SetTitleOffset(1.5);

    double yMin = g->GetYaxis()->GetXmin();
    double yMax = g->GetYaxis()->GetXmax();
    double xRangeMin = sampleTimes[0];
    double xRangeMax = sampleTimes.back();
    std::unique_ptr<TPaveText> txtbox(new TPaveText(
		xRangeMin + (xRangeMax - xRangeMin) * 0.6, yMin + (yMax - yMin) * 0.6,
		xRangeMin + (xRangeMax - xRangeMin) * 0.9, yMin + (yMax - yMin) * 0.35));
    txtbox->SetFillColor(kWhite);
    func->SetParameter(6, frr[0].pedestal);
    for (int i = 0; i < nPulses; ++i) {
	func->SetParameter(2 * i, frr[i].time);
	txtbox->AddText(Form("t_{%i}: %.3f", i + 1, frr[i].time));
	func->SetParameter(2 * i + 1, -1 * frr[i].energy);
	txtbox->AddText(Form("E_{%i}: %.0f", i + 1, frr[i].energy));
    }
    txtbox->AddText(Form("pedestal: %.0f", frr[0].pedestal));
    txtbox->AddText(Form("#chi^{2} / NDF : %.2f", frr[0].chi2));

    std::vector<std::unique_ptr<TF1>> components;
    if (nPulses > 1) {
	int colors[3] = {kRed, kBlue, kGreen + 2};
	for (int i = 0; i < nPulses; ++i) {
	    components.emplace_back(
		    new TF1("fitFunc", templateFunction, 0, sampleTimes.back(), 7));
	    components.back()->SetParameters(std::vector<double>(7, 0).data());
	    components.back()->SetParameter(6, frr[0].pedestal);
	    components.back()->SetParameter(2 * i, frr[i].time);
	    components.back()->SetParameter(2 * i + 1, -1 * frr[i].energy);
	    components.back()->SetLineColor(colors[i]);
	    components.back()->SetNpx(100000);
	    components.back()->Draw("same");
	}
    }

    func->SetNpx(100000);
    func->Draw("same");
    func->SetLineColor(kMagenta + 2);
    txtbox->Draw("same");
    //	c->Print("lastFit.pdf");
    c->Print(filename.c_str());

    c->Modified();
    c->Update();
    c->Draw();
    gSystem->ProcessEvents();

    c->Write();
    lastCanvas.Write();
    lastCanvas.Close();

 //  std::cout << title << " displayed. Any key to move on" << std::endl;
//   std::cin.ignore();
}
