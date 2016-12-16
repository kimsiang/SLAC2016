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


void fitWaveform(TemplateFitter& tf, const gm2calo::IslandArtRecord, TSpline3* tSpline);

int main(int argc, char const* argv[]) {

    // declare new TApplication
    new TApplication("app", 0, nullptr);

    TString splinePath = "beamTemplateFile24.root";
    TString splinePath2 = "beamleakTemplateFile24.root";


    // get TSpline for main shower
    TFile templateFile(splinePath);
    std::unique_ptr<TSpline3> tSpline((TSpline3*)templateFile.Get("masterSpline"));

    // get TSpline for leaked shower
    TFile templateFile2(splinePath2);
    std::unique_ptr<TSpline3> tSpline2((TSpline3*)templateFile2.Get("masterSpline"));

    // generate a waveform from the TSpline
    vector<short> trace;

    for(int i=0;i<40;i++){
        trace.push_back(5000*tSpline2->Eval(i-10));
    }

    // store the waveform inside an IslandArtRecord
    gm2calo::IslandArtRecord thisIsland; 
    thisIsland.trace = trace;
    thisIsland.firstSampleNum = 0;;
 
    TemplateFitter tf;
    tf.setTemplate(tSpline.get(), tSpline->GetXmin(), tSpline->GetXmax(), 10000);
    tf.setMaxIterations(200);

    fitWaveform(tf, thisIsland, tSpline.get());

}

void fitWaveform(TemplateFitter& newFitter, const gm2calo::IslandArtRecord thisIsland, TSpline3* tSpline){

    // make plot
    std::string title = "Test";
    std::unique_ptr<TCanvas> c(
            new TCanvas("c", (title + "_canvas").c_str(), 1200, 1200));
    std::unique_ptr<TGraph> g(new TGraph(0));

    vector<short> trace = thisIsland.trace;

    vector<short> sampleTimes;
    for(int i=0;i<trace.size();i++){
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
    func->SetParameter(6, newFitResults[0].pedestal);
    func->SetParameter(0, newFitResults[0].time);
    txtbox->AddText(Form("t_{%i}: %.3f", 1, newFitResults[0].time));
    func->SetParameter(1, newFitResults[0].energy);
    txtbox->AddText(Form("E_{%i}: %.0f", 1, newFitResults[0].energy));
    txtbox->AddText(Form("pedestal: %.0f", newFitResults[0].pedestal));
    txtbox->AddText(Form("#chi^{2} / NDF : %.2f", newFitResults[0].chi2));

    func->SetNpx(10000);
    func->Draw("same");
    func->SetLineColor(kMagenta + 2);
    txtbox->Draw("same");
    //	c->Print("lastFit.pdf");
    //c->Print(filename.c_str());

    c->Modified();
    c->Update();
    c->Draw();
    gSystem->ProcessEvents();

    //    c->Write();

    cin.ignore();
}
