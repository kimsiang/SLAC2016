void plot(){

    gStyle->SetOptStat(0);

    int nbin= 2000;
    TH1D *outputBeam = new TH1D("outputBeam","outputBeam",nbin,0,20);
    TH1D *outputBeamSim = new TH1D("outputBeamSim","outputBeamSim",nbin,0,20);
    TH1D *outputLaser = new TH1D("outputLaser","outputLaser",nbin,0,20);
    TH1D *outputLaserOnSiPM = new TH1D("outputLaserOnSiPM","outputLaserOnSiPM",nbin,0,20);
    TH1D *input = new TH1D("input","input",nbin,0,20);

    double laser[nbin];
    double source[nbin];
    double fitTemp[nbin];
    double resp[nbin];

    TCanvas *c1 = new TCanvas("c1","c1",1200,800);
    c1->Divide(2,2);

    // Photon distribution
    TFile *distFile = new TFile("photonDistribution.root");
    TH1D *photon = (TH1D*)distFile->Get("h2");
    photon->Scale(50./photon->GetEntries());
    photon->SetLineColor(1);
    //   photon->Draw();
    photon->GetXaxis()->SetTitle("time [ns]");

    for(int i=0;i<nbin;i++){
        resp[i]=photon->GetBinContent(i+1);
    }

    // Laser on SiPM
    TFile *laserSFile = new TFile("pulseShape.root");
    TSpline *tSplineLaserS =(TSpline3*)laserSFile->Get("masterSpline");

    for(int i=0;i<nbin;i++){
        laser[i] = tSplineLaserS->Eval((-8.3 + (double)i/100.0));
        outputLaserOnSiPM->SetBinContent(i + 1,laser[i]);
    }

    //   outputLaserOnSiPM->Draw();
    outputLaserOnSiPM->SetLineColor(4);
    outputLaserOnSiPM->SetLineStyle(7);

    // Fit Template
    TFile *fitTFile = new TFile("fitTemplate.root");
    TSpline *tSplineFitT =(TSpline3*)fitTFile->Get("masterSpline");

    for(int i=0;i<nbin;i++){
        fitTemp[i] = tSplineFitT->Eval((-8.8 + (double)i/100.0)/1.25);
        outputBeamSim->SetBinContent(i + 1,fitTemp[i]);
    }

    //   outputBeamSim->Draw("same");
    outputBeamSim->SetLineColor(3);
    outputBeamSim->SetLineStyle(7);

    // Laser Template
    TFile *laserFile = new TFile("laserTemplates/laserTemplateFile44.root");
    TSpline *tSplineLaser =(TSpline3*)laserFile->Get("masterSpline");

    for(int i=0;i<nbin;i++){
        laser[i] = tSplineLaser->Eval((-8.3 + (double)i/100.0)/1.25);
        outputLaser->SetBinContent(i + 1,laser[i]);
    }

    //   outputLaser->Draw("same");
    outputLaser->SetLineColor(4);

    // Beam Template
    TFile *sourceFile = new TFile("beamTemplates/beamTemplateFile33.root");
    TSpline *tSpline =(TSpline3*)sourceFile->Get("masterSpline");

    for(int i=0;i<nbin;i++){
        source[i] = tSpline->Eval((-8.8 + (double)i/100.0)/1.25);
        outputBeam->SetBinContent(i + 1,source[i]);
    }

    //   outputBeam->Draw("same");
    outputBeam->SetLineColor(2);

    // Deconvolution here
    TSpectrum *s = new TSpectrum();
    s->Deconvolution(source,resp,nbin,100,1,1);

    // Put the unfolded response back
    for (int i = 0; i < nbin; i++) input->SetBinContent(i + 1,source[i]);
    outputBeam->SetLineColor(3);
    input->SetLineColor(2);
    //   input->Draw("same");

    photon->SetLineWidth(2);
    input->SetLineWidth(2);
    outputBeam->SetLineWidth(2);
    outputBeamSim->SetLineWidth(2);
    outputLaser->SetLineWidth(2);
    outputLaserOnSiPM->SetLineWidth(2);

    photon->GetXaxis()->SetTitle("time [ns]");
    input->GetXaxis()->SetTitle("time [ns]");
    outputBeam->GetXaxis()->SetTitle("time [ns]");
    outputBeamSim->GetXaxis()->SetTitle("time [ns]");
    outputLaser->GetXaxis()->SetTitle("time [ns]");
    outputLaserOnSiPM->GetXaxis()->SetTitle("time [ns]");


    c1->cd(1);
    photon->Draw();
    outputBeam->Draw("same");
    input->Draw("same");

    TLegend *leg1 = new TLegend(0.5,0.6,0.9,0.9);
    leg1->AddEntry(photon,"PhotonDist (sim)","l");
    leg1->AddEntry(outputBeam,"BeamTemplate (data)","l");
    leg1->AddEntry(input,"Response (decon.)","l");
    leg1->Draw();


    c1->cd(2);
    input->Draw();
    outputLaserOnSiPM->Draw("same");

    TLegend *leg2 = new TLegend(0.5,0.6,0.9,0.9);
    leg2->AddEntry(input,"Response (decon.)","l");
    leg2->AddEntry(outputLaserOnSiPM,"Response (sim, LaserOnSiPM)","l");
    leg2->Draw();


    c1->cd(3);
    outputBeam->Draw();
    outputBeamSim->Draw("same");
    outputBeam->Scale(1.27);

    TLegend *leg3 = new TLegend(0.5,0.6,0.9,0.9);
    leg3->AddEntry(outputBeam,"BeamTemplate (data)","l");
    leg3->AddEntry(outputBeamSim,"BeamTemplate (sim)","l");
    leg3->Draw();

    c1->cd(4);
 //  input->Draw();
    outputLaser->Draw();
    outputLaser->Scale(0.915);
    outputLaserOnSiPM->Draw("same");

    TLegend *leg4 = new TLegend(0.5,0.6,0.9,0.9);
//    leg4->AddEntry(input,"Response (decon.)","l");
    leg4->AddEntry(outputLaserOnSiPM,"Response (sim, LaserOnSiPM)","l");
    leg4->AddEntry(outputLaser,"LaserTemplate (data)","l");
    leg4->Draw();

}

