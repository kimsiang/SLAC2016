#include "../include/TemplateFitter.hh"
#include "../include/TemplateFitApplier.hh"

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
#include "TH2.h"
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

bool drawing = false;

gm2calo::FitResultArtRecord fitWaveform(TemplateFitter& tf, const gm2calo::IslandArtRecord, TSpline3* tSpline);

int main(int argc, char const* argv[]) {

    if(argc > 1){
	cout<<"There should be no argument!!"<<endl;
	return 1;
    }

    // declare new TApplication
    new TApplication("app", 0, nullptr);

    TString splinePath = "beamTemplateFile37.root";
    TString splinePath2 = "beamleakTemplateFile37.root";

    // get TSpline for main shower
    TFile templateFile(splinePath);
    std::unique_ptr<TSpline3> tSpline((TSpline3*)templateFile.Get("masterSpline"));

    // get TSpline for leaked shower
    TFile templateFile2(splinePath2);
    std::unique_ptr<TSpline3> tSpline2((TSpline3*)templateFile2.Get("masterSpline"));

    // create template fitter
    TemplateFitter tf;
    tf.setTemplate(tSpline.get(), tSpline->GetXmin(), tSpline->GetXmax(), 50000);
    tf.setMaxIterations(200);

    // generate a waveform from the TSpline
    vector<short> trace;

    // Loop through some randomized energies
    int nTry=10000;

    // 2D histogram to compare input and output
    TH2D *compareE = new TH2D("compareE","compareE",100,999,1001,100,999,1001);
    TH2D *compareT = new TH2D("compareT","compareT",100,9.9,10.1,100,9.9,10.1);
    compareE->GetXaxis()->SetCanExtend(kTRUE);
    compareE->GetYaxis()->SetCanExtend(kTRUE);
    compareT->GetXaxis()->SetCanExtend(kTRUE);
    compareT->GetYaxis()->SetCanExtend(kTRUE);
    

    for(int iTry=0;iTry<nTry;iTry++){

	// empty trace
	trace.clear();

	// generate a random time
	double time = 10 + (gRandom->Rndm() - 0.5);

	for(int i=0;i<40;i++){
	    trace.push_back(1000*tSpline->Eval(i-time));
	}

	// store the waveform inside an IslandArtRecord
	gm2calo::IslandArtRecord thisIsland; 
	thisIsland.trace = trace;
	thisIsland.firstSampleNum = 0;;

	auto fitResults = fitWaveform(tf, thisIsland, tSpline.get()); 

	compareE->Fill(1000,fitResults.energy);
	compareT->Fill(time,fitResults.time);
    }

    TCanvas c1("c1","c1",1200,600);
    c1.Divide(2,1);
    c1.cd(1);
    compareE->Draw("colz");
    c1.cd(2);
    compareT->Draw("colz");
    c1.Modified();
    c1.Update();
    c1.Draw();
    gSystem->ProcessEvents();
    cin.ignore();

    return 0;
}

gm2calo::FitResultArtRecord fitWaveform(TemplateFitter& newFitter, const gm2calo::IslandArtRecord thisIsland, TSpline3* tSpline){

    // make plot
    std::string title = "Test";

//   std::unique_ptr<TCanvas> c(
//	    new TCanvas("c", (title + "_canvas").c_str(), 1200, 1200));

    std::unique_ptr<TGraph> g(new TGraph(0));

    vector<short> trace = thisIsland.trace;

    vector<short> sampleTimes;
    for(size_t i=0;i<trace.size();i++){
	sampleTimes.push_back(i + thisIsland.firstSampleNum);
    }

    for (size_t i = 0; i < trace.size(); ++i) {
	g->SetPoint(g->GetN(), sampleTimes[i], trace[i]);
    }

    gm2calo::TemplateFitApplier applyTemplateFit_(false, 20, 2, 10, 0.025, 30, 3);
    auto newFitResults = applyTemplateFit_(newFitter, thisIsland);

    double xMin = tSpline->GetXmin();
    double xMax = tSpline->GetXmax();
    auto templateFunction = [&](double* x, double* p) {
	double returnValue = p[6];
	if ((x[0] - p[0] > xMin) && (x[0] - p[0] < xMax)) {
	    returnValue += p[1] * tSpline->Eval(x[0] - p[0]);
	}
	return returnValue;
    };

    std::unique_ptr<TF1> func(new TF1("fitFunc", templateFunction, sampleTimes[0],
		sampleTimes.back(), 7));
    func->SetLineColor(kBlack);
    func->SetParameters(std::vector<double>(7, 0).data());

    g->SetMarkerStyle(20);
//   g->Draw("ap");
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
    func->SetParameter(6, newFitResults[0].pedestal);
    func->SetParameter(0, newFitResults[0].time);
    txtbox->AddText(Form("t_{%i}: %.3f", 1, newFitResults[0].time));
    func->SetParameter(1, newFitResults[0].energy);
    txtbox->AddText(Form("E_{%i}: %.0f", 1, newFitResults[0].energy));
    txtbox->AddText(Form("pedestal: %.0f", newFitResults[0].pedestal));
    txtbox->AddText(Form("#chi^{2} / NDF : %.2f", newFitResults[0].chi2));

    func->SetNpx(10000);
//   func->Draw("same");
    func->SetLineColor(kMagenta + 2);
//    txtbox->Draw("same");

/*    if(drawing){
	c->Modified();
	c->Update();
	c->Draw();
	gSystem->ProcessEvents();
	//cin.ignore();
    }
*/
    return newFitResults[0];
}
