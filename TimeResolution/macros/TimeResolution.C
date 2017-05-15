////////////////////////////////////
// Time Resolution study
//
// To run the macro,
// root TimeResolution.C
//
// Kim Siang Khaw, khaw84@uw.edu
//
////////////////////////////////////

// For effective energy calculation
double eff(double E1, double E2){
  return E1*E2/sqrt(E1*E1+E2*E2);
}

// For pixel saturation correction
double eReal(double e){
  double nPixel = 57344.;
  return nPixel*log(1-e/nPixel);
}

int wfd5Num(int ch){
  int wfd5Map[11] = {7, 10, 4, 9, 3, 8, 11, 2, 1, 6, 5};
  int num = 0;

  if(ch<45) num = wfd5Map[ch % 9];
  else if(ch>=45 && ch < 50) num = wfd5Map[9];
  else num = wfd5Map[10];

  return num;
}

// For checking if both channels are from the same WFD5
bool isSameWFD5(int ch1, int ch2){

  bool same = false;
  if(wfd5Num(ch1) == wfd5Num(ch2)){
    same = true;
  }

  return same;
}


void TimeResolution(){

  // set important const variables here 
  int startRun = 1751; // 1751 or 2133 for now
  const int nRun = 8;
  const int nXtal = 54;

  // files to open
  TFile *f[nRun];

  for(int i=0;i<nRun;i++){
    f[i] = new TFile(Form("../data/gm2slac_run0%d.root.root",startRun+i)); 
  }

  // define all variables and TObjects 
  double energy[nXtal][nRun];
  double effEnergy[nXtal][nXtal][nRun];
  double resolution[nXtal][nXtal][nRun];
  TH1D *syncE[nXtal][nRun];
  TH1D *syncTimeDiff[nXtal][nXtal][nRun];

  // plotting E and dt distributions for sanity check
  // TCanvas *c[8];

  // loop for each run
  for(int i=0;i<nRun;i++){

    cout<<"File["<<i+1<<"/"<<nRun<<"] Run: "<<i+startRun<<endl;
    // c[i] = new TCanvas(Form("c%d",i),Form("c%d",i),1200,900);
    //  c[i]->Divide(9,6);

    // loop for each crystal
    for(int j=0;j<nXtal;j++){

      //   c[i]->cd(54-j);
      f[i]->GetObject(Form("syncEnergy%02d",j), syncE[j][i]);

      // get initial parameters for the energy fit
      double amp = syncE[j][i]->GetMaximum();
      double mean = syncE[j][i]->GetBinCenter(syncE[j][i]->GetMaximumBin());
      double sigma = syncE[j][i]->GetRMS();

      // now fit energy distribution, 1st fit
      TF1 fit("fit","gaus(0)",mean-3*sigma,mean+3*sigma);
      fit.SetParameters(amp,mean,sigma);
      syncE[j][i]->Fit("fit","QMERN");
      amp = fit.GetParameter(0);
      mean = fit.GetParameter(1);
      sigma = fit.GetParameter(2);

      // now fit energy distribution, 2nd fit
      fit.SetParameters(amp,mean,sigma);
      syncE[j][i]->Fit("fit","QMERN");
      mean = fit.GetParameter(1);
      sigma = fit.GetParameter(2);

      // adjust range for better plotting
      // syncE[j][i]->GetXaxis()->SetRangeUser(mean-5*sigma, mean+5*sigma);

      // get pixel saturation corrected energy
      energy[j][i] = eReal(mean);

      cout<<"file["<<i+1<<"/"<<nRun<<"]   xtal: "<<j<<", ene: "<<mean<<", sigma: "<<sigma
        <<" (chi2/NDF: "<<fit.GetChisquare()<<"/"<<fit.GetNDF()<<")"<<endl;

      // loop for each crystal to compare with
      for(int k=0;k<nXtal;k++){

        // (j,k) is the same with (k,j)
        if(k>=j) continue;

        f[i]->GetObject(Form("syncTimeDiff%02d%02d",j,k),syncTimeDiff[k][j][i]);
        syncTimeDiff[k][j][i]->SetTitle(Form("Run%dTimeDiff%02d%02d",i+startRun,j,k));

        // get initial parameters for the dt fit, assume sigma = 0.03
        mean = syncTimeDiff[k][j][i]->GetBinCenter(syncTimeDiff[k][j][i]->GetMaximumBin());
        sigma = 0.03;

        // now fit dt, 1st fit
        TF1 fit("fit","gaus(0)",mean-3*sigma,mean+3*sigma);
        fit.SetParameters(200,mean,0.1);
        syncTimeDiff[k][j][i]->Fit("fit","QMERN");
        mean = fit.GetParameter(1);
        sigma = fit.GetParameter(2);

        // now fit dt, 2nd fit
        fit.SetParameters(200,mean,sigma);
        syncTimeDiff[k][j][i]->Fit("fit","QMERN");
        mean = fit.GetParameter(1);
        sigma = fit.GetParameter(2);

        cout<<"file["<<i+1<<"/"<<nRun<<"]     dT("<<j<<","<<k<<"): "<<mean
          <<", sigma: "<<sigma<<" (chi2/NDF: "<<fit.GetChisquare()<<"/"<<fit.GetNDF()<<")"<<endl;

        resolution[k][j][i]=sigma;
        effEnergy[k][j][i]=eff(energy[k][i],energy[j][i]);

        delete syncTimeDiff[k][j][i];
      }
      delete syncE[j][i];  
    }
    // c[i]->Print(Form("syncE_run%d.pdf",startRun+i));
    delete f[i];
  }

  // now fit the dt vs E_{eff} plots
  TF1 *myfit = new TF1("myfit", "(2*[0]^2+ [1]^2/(x/5.0))^0.5", 0, 1800);
  myfit->SetParameter(0, 0.05);
  myfit->SetParameter(1, 0.4);
  myfit->SetParLimits(0, 0, 0.2);
  myfit->SetParLimits(1, 0, 1);

  // store all possible pairs 54 C 2 = 1431 when nXtal = 54
  std::vector<std::pair<int, int>> sipmPair;

  for(int i=0;i<nXtal;i++){
    for(int j=0;j<nXtal;j++){
      if(i>=j) continue;
      sipmPair.push_back(make_pair(i,j)); 
    }
  }

  // histograms for fitted C and S terms for same and diff WFD5
  TH1D *h_C1 = new TH1D("C_term_same", "C_term_same",200,0,0.04); 
  TH1D *h_C2 = new TH1D("C_term_diff", "C_term_diff",200,0,0.04); 
  TH1D *h_S1 = new TH1D("S_term_same", "S_term_same",200,0.3,0.6); 
  TH1D *h_S2 = new TH1D("S_term_diff", "S_term_diff",200,0.3,0.6); 

  // define TGraphs and later store them in a TMultiGraph
  TGraph *g[sipmPair.size()];
  TMultiGraph *mg = new TMultiGraph();

  for(int i=0;i<sipmPair.size();i++){

    int ch1 = sipmPair.at(i).first;
    int ch2 = sipmPair.at(i).second;

    g[i] = new TGraph(nRun,effEnergy[ch1][ch2], resolution[ch1][ch2]);
    g[i]->SetMarkerStyle(20);

    bool sameWFD5 = isSameWFD5(ch1,ch2);

    if(sameWFD5){
      g[i]->SetMarkerColor(kAzure-ch1%9+4);
      myfit->SetLineColor(kAzure-ch1%9+4);
    }

    else{
      g[i]->SetMarkerColor(kPink-ch1%9+4);
      myfit->SetLineColor(kPink-ch1%9+4);
    }

    /// Reinitialize fit parameters for each fit 
    myfit->SetParameter(0, 0.05);
    myfit->SetParameter(1, 0.4);
    myfit->SetParLimits(0, 0, 0.2);
    myfit->SetParLimits(1, 0, 1);
    myfit->SetNpx(2000);
    g[i]->Fit(myfit,"REMQ");

    double cTerm = myfit->GetParameter(0);
    double sTerm = myfit->GetParameter(1);

    if(sameWFD5){
      h_C1->Fill(cTerm);
      h_S1->Fill(sTerm);
    }

    else {
      h_C2->Fill(cTerm);
      h_S2->Fill(sTerm);
    }

    //leg->AddEntry(g[i],Form("dt(%d,%d),%.4f,%.4f",sipmPair.at(i).first,sipmPair.at(i).second,term1,term2),"p");
    mg->Add(g[i]);
    g[i]->Clear();
  }

  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  c1->cd();

  mg->SetTitle(Form("dt(i,j) vs E_{eff}"));
  mg->Draw("AP");
  mg->GetYaxis()->SetRangeUser(0.01,0.1);
  mg->GetXaxis()->SetLimits(0,1800);

  // leg->SetNColumns(5);
  // leg->Draw();

  // Another canvas to plot the fitted C and S distributions
  TCanvas *c2 = new TCanvas("c2","c2",1000,600);

  c2->Divide(2,1);
  c2->cd(1);
  h_C2->SetLineColor(2);
  h_C2->SetLineWidth(2);
  h_C2->Draw();
  h_C2->GetYaxis()->SetRangeUser(0,1.2*h_C2->GetBinContent(h_C2->GetMaximumBin()));
  h_C2->GetXaxis()->SetTitle("C [c.t.]");
  h_C2->GetYaxis()->SetTitle("count/0.0004 c.t.");
  h_C2->SetTitle("");
  h_C1->SetLineColor(4);
  h_C1->SetLineWidth(2);
  h_C1->Draw("same");

  TLegend *legC = new TLegend(0.6,0.7,0.89,0.89);
  legC->AddEntry(h_C1,"Same WFD5","l");
  legC->AddEntry(h_C2,"Diff WFD5","l");
  legC->SetLineColor(0);
  legC->Draw();

  c2->cd(2);
  h_S2->SetLineColor(2);
  h_S2->SetLineWidth(2);
  h_S2->Draw();
  h_S2->SetTitle("");
  h_S2->GetYaxis()->SetRangeUser(0,1.2*h_S2->GetBinContent(h_S2->GetMaximumBin()));
  h_S2->GetXaxis()->SetTitle("S [c.t.]");
  h_S2->GetYaxis()->SetTitle("count/0.003 c.t.");
  h_S1->SetLineColor(4);
  h_S1->SetLineWidth(2);
  h_S1->Draw("same");

  TLegend *legS = new TLegend(0.6,0.7,0.89,0.89);
  legS->AddEntry(h_S1,"Same WFD5","l");
  legS->AddEntry(h_S2,"Diff WFD5","l");
  legS->SetLineColor(0);
  legS->Draw();

  gStyle->SetOptStat(0);

}


