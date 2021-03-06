#include "../include/TemplateFitter.hh"
#include "../include/TemplateFitApplier.hh"

#include "TApplication.h"
#include "TSystem.h"
#include "TFile.h"
#include "TSpline.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TPaveText.h"
#include "TString.h"
#include "TAxis.h"
#include "TF1.h"
#include "TH2.h"
#include "TTree.h"
#include "TColor.h"

#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <numeric>
#include <memory>

using namespace std;

bool drawing = true;

gm2calo::FitResultArtRecord fitWaveform(TemplateFitter& tf, const gm2calo::IslandArtRecord, TSpline3* tSpline);

int main(int argc, char const* argv[]) {

    if(argc < 2){
        cout<<"Usage ./crossFitter nTry"<<endl;
        return 1;
    }

    // declare new TApplication
    new TApplication("app", 0, nullptr);
    gStyle->SetLabelSize(0.05,"xy");
    gStyle->SetTitleSize(0.05,"xy");

    TString splinePath;
    TString splinePath2;

    if(atoi(argv[2]) == 1){
        splinePath = "beamTemplateFile37.root";
    }
    else if(atoi(argv[2]) == 2){
        splinePath = "beamleakTemplateFile37.root";
    }
    else{
        cout<<"Please use only template 1(A) or 2(B)!"<<endl;
        return 2;
    }

    if(atoi(argv[3]) == 1){
        splinePath2 = "beamTemplateFile37.root";
    }
    else if(atoi(argv[3]) == 2){
        splinePath2 = "beamleakTemplateFile37.root";
    }
    else{
        cout<<"Please use only sample 1(A) or 2(B)!"<<endl;
        return 3;
    }



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
    int nTry = atoi(argv[1]);

    // 2D histogram to compare input and output
    TH2D *compareE = new TH2D("compareE","E_{fitted} vs E_{input}",500,0,8000,500,0,8000);
    TH2D *compareT = new TH2D("compareT","T_{fitted} vs T_{input}",500,9.5,10.5,500,9.5,10.5);
    compareE->GetXaxis()->SetTitle("E_{input}");
    compareE->GetYaxis()->SetTitle("E_{fitted}");
    compareT->GetXaxis()->SetTitle("T_{input}");
    compareT->GetYaxis()->SetTitle("T_{fitted}");

    //   compareE->GetYaxis()->SetCanExtend(kTRUE);
    //   compareT->GetYaxis()->SetCanExtend(kTRUE);

    TH2D *timeDiff = new TH2D("timeDiff","T_{fitted-input} vs E_{input}",500,0,8000,500,-0.06,0.06); 
    TH2D *energyRatio = new TH2D("energyRatio","(E_{fitted}-E_{input})/E_{input} vs E_{input}",500,0,8000,500,-0.06,0.06); 
    timeDiff->GetXaxis()->SetTitle("E_{input}");
    timeDiff->GetYaxis()->SetTitle("E_{input} vs T_{fitted-input}");
    energyRatio->GetXaxis()->SetTitle("E_{input}");
    energyRatio->GetYaxis()->SetTitle("(E_{fitted}-E_{input})/E_{input}");

    //   timeDiff->GetYaxis()->SetCanExtend(kTRUE);
    //   energyRatio->GetYaxis()->SetCanExtend(kTRUE);

    for(int iTry=0;iTry<nTry;iTry++){

        // empty trace
        trace.clear();

        // generate a random time and energy
        double time = 10 + (gRandom->Rndm() - 0.5);
        double energy = 7000 * (0.05 + gRandom->Rndm());

        for(int i=0;i<40;i++){
            trace.push_back(energy*tSpline2->Eval(i-time));
        }

        // store the waveform inside an IslandArtRecord
        gm2calo::IslandArtRecord thisIsland; 
        thisIsland.trace = trace;
        thisIsland.firstSampleNum = 0;

        auto fitResults = fitWaveform(tf, thisIsland, tSpline.get()); 

        compareE->Fill(energy,fitResults.energy);
        compareT->Fill(time,fitResults.time);
        energyRatio->Fill(energy, (fitResults.energy-energy)/energy );
        timeDiff->Fill(energy, (fitResults.time-time) );

    }

    TCanvas c1("c1","c1",1200,800);
    c1.Divide(2,2);
    c1.cd(1);
    compareE->Draw("colz");
    c1.cd(2);
    compareT->Draw("colz");
    c1.cd(3);
    energyRatio->Draw("colz");
    c1.cd(4);
    timeDiff->Draw("colz");


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

    std::unique_ptr<TCanvas> c(
            new TCanvas("c", (title + "_canvas").c_str(), 1200, 1200));

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

    if(drawing){
        c->Modified();
        c->Update();
        c->Draw();
        gSystem->ProcessEvents();
        cin.ignore();
    }

    return newFitResults[0];
}
