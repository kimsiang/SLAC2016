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
  int startRun = 2133;
  const int nRun = 8;
  const int nXtal = 10;

  // files to open
  TFile *f[nRun];

  for(int i=0;i<nRun;i++){
    f[i] = new TFile(Form("gm2slac_run0%d.root.root",startRun+i)); 
  }

  // define all variables and TObjects 
  double sigma[nXtal][nXtal][nRun];
  double energy[nXtal][nRun];
  double effEnergy[nXtal][nXtal][nRun];
  TH1D *syncE[nXtal][nRun];
  TH1D *syncTimeDiff[nXtal][nXtal][nRun];
  TF1 *fit;

  // plotting E and dt distributions for sanity check
  // TCanvas *c[8];

  // loop for each run
  for(int i=0;i<nRun;i++){

    // c[i] = new TCanvas(Form("c%d",i),Form("c%d",i),1200,900);
    // c[i]->Divide(9,6);

    // loop for each crystal
    for(int j=0;j<nXtal;j++){

      // c[i]->cd(54-j);
      syncE[j][i] = (TH1D*) f[i]->Get(Form("syncEnergy%02d",j));

      // get initial parameters for the energy fit
      double mean = syncE[j][i]->GetBinCenter(syncE[j][i]->GetMaximumBin());
      double rms = syncE[j][i]->GetRMS();

      // now fit energy, 1st trial
      fit = new TF1("fit","gaus(0)",mean-3*rms,mean+3*rms);
      fit->SetParameters(500,mean,rms);
      syncE[j][i]->Fit("fit","QMERN");
      mean = fit->GetParameter(1);
      rms = fit->GetParameter(2);
      fit->Clear(); 

      // now fit energy, 2nd trial
      fit = new TF1("fit","gaus(0)",mean-3*rms,mean+3*rms);
      fit->SetParameters(500,mean,rms);
      syncE[j][i]->Fit("fit","QMERN");
      mean = fit->GetParameter(1);
      rms = fit->GetParameter(2);
      fit->Clear(); 

      // get pixel saturation corrected energy
      energy[j][i] = eReal(mean);

      cout<<"Run: "<<i+startRun<<", Xtal: "<<j<<", E: "<<mean<<"("<<fit->GetChisquare()/fit->GetNDF()<<")"<<endl;

      // loop for each crystal to compare with
      for(int k=0;k<nXtal;k++){

        // (j,k) is the same with (k,j)
        if(k>=j) continue;

        syncTimeDiff[k][j][i] = (TH1D*) f[i]->Get(Form("syncTimeDiff%02d%02d",j,k));
        syncTimeDiff[k][j][i]->SetTitle(Form("Run%dTimeDiff%02d%02d",i+startRun,j,k));

        // get initial parameters for the dt fit, assume rms = 0.03
        mean = syncTimeDiff[k][j][i]->GetBinCenter(syncTimeDiff[k][j][i]->GetMaximumBin());
        rms = 0.03;

        // now fit dt, 1st trial
        fit = new TF1("fit","gaus(0)",mean-3*rms,mean+3*rms);
        fit->SetParameters(200,mean,0.1);
        syncTimeDiff[k][j][i]->Fit("fit","QMERN");
        mean = fit->GetParameter(1);
        rms = fit->GetParameter(2);
        fit->Clear(); 

        // now fit dt, 2nd trial
        fit = new TF1("fit","gaus(0)",mean-3*rms,mean+3*rms);
        fit->SetParameters(200,mean,rms);
        syncTimeDiff[k][j][i]->Fit("fit","QMERN");
        mean = fit->GetParameter(1);
        rms = fit->GetParameter(2);
        fit->Clear(); 

        // now fit dt, 3rd trial
        fit = new TF1("fit","gaus(0)",mean-3*rms,mean+3*rms);
        fit->SetParameters(200,mean,rms);
        syncTimeDiff[k][j][i]->Fit("fit","QMERN");
        mean = fit->GetParameter(1);
        rms = fit->GetParameter(2);
        fit->Clear(); 

        cout<<"Run: "<<i+startRun<<", dT("<<j<<","<<k<<"): "<<mean<<"("<<fit->GetChisquare()<<"/"<<fit->GetNDF()<<")"<<endl;

        sigma[k][j][i]=rms;
        effEnergy[k][j][i]=eff(energy[k][i],energy[j][i]);
      }
    }
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
  TH1D *h_S1 = new TH1D("S_term_same", "S_term_same",200,0.3,0.55); 
  TH1D *h_S2 = new TH1D("S_term_diff", "S_term_diff",200,0.3,0.55); 


  TMultiGraph  *mg  = new TMultiGraph();
  TGraph *g[sipmPair.size()];

  for(int i=0;i<sipmPair.size();i++){
    g[i] = new TGraph(nRun,effEnergy[sipmPair.at(i).first][sipmPair.at(i).second],
        sigma[sipmPair.at(i).first][sipmPair.at(i).second]);
    g[i]->SetMarkerStyle(20);

    bool sameWFD5 = isSameWFD5(sipmPair.at(i).first,sipmPair.at(i).second);

    if(sameWFD5){
      g[i]->SetMarkerColor(kAzure-sipmPair.at(i).first%9+4);
      myfit->SetLineColor(kAzure-sipmPair.at(i).first%9+4);
    }
    else{
      g[i]->SetMarkerColor(kPink-sipmPair.at(i).first%9+4);
      myfit->SetLineColor(kPink-sipmPair.at(i).first%9+4);
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
  }

  TCanvas *c1 = new TCanvas("c1","c1",1000,600);
  c1->cd();

  mg->Draw("AP");
  mg->SetTitle(Form("dt(i,j) vs E_{eff}"));
  mg->GetYaxis()->SetRangeUser(0.01,0.08);
  mg->GetXaxis()->SetLimits(0,1800);

  // leg->SetNColumns(5);
  // leg->Draw();

  // Another canvas to plot the fitted C and S distributions
  TCanvas *c2 = new TCanvas("c2","c2",1000,600);

  c2->Divide(2,1);
  c2->cd(1);
  h_C2->SetLineColor(4);
  h_C2->SetLineWidth(2);
  h_C2->Draw();
  h_C2->GetXaxis()->SetTitle("C [c.t.]");
  h_C2->SetTitle("");
  h_C1->SetLineColor(2);
  h_C1->SetLineWidth(2);
  h_C1->Draw("same");

  TLegend *legC = new TLegend(0.6,0.7,0.89,0.89);
  legC->AddEntry(h_C1,"Same WFD5","l");
  legC->AddEntry(h_C2,"Diff WFD5","l");
  legC->SetLineColor(0);
  legC->Draw();

  c2->cd(2);
  h_S2->SetLineColor(4);
  h_S2->SetLineWidth(2);
  h_S2->Draw();
  h_S2->SetTitle("");
  h_S2->GetXaxis()->SetTitle("S [c.t.]");
  h_S1->SetLineColor(2);
  h_S1->SetLineWidth(2);
  h_S1->Draw("same");

  TLegend *legS = new TLegend(0.6,0.7,0.89,0.89);
  legS->AddEntry(h_S1,"Same WFD5","l");
  legS->AddEntry(h_S2,"Diff WFD5","l");
  legS->SetLineColor(0);
  legS->Draw();

  gStyle->SetOptStat(0);
}


