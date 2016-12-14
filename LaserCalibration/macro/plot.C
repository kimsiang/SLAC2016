void plot(){

    gStyle->SetTitleH(0.1);   

    TFile *f[53];

    ifstream infile("root.list");

    vector<string> filename(53);

    int nfile = 0;

    while(infile >> filename[nfile]) {
        f[nfile] = new TFile(Form("%s",filename[nfile].c_str()));
        nfile++;
    }

    TCanvas *c1 = new TCanvas("c1","c1",1800,1200);

    TH2D *constants[54];
    for(int i=0;i<54;i++){
        constants[i] = new TH2D(Form("constants%d",i),Form("constants%d",i),20,0,1,20,0,100);
    }

    double calibConst[54][53];
    double calibConst1751[54];

    double runNum[53];
    double xtalNum[54];


    c1->Divide(9,6);

    for(int j=0;j<53;j++){

        runNum[j] = stoi(filename[j].substr(22,4).c_str()); 

        for(int i=0;i<54;i++){
            xtalNum[i]=i;
            c1->cd(54-i);
            TGraph *g = (TGraph*)f[j]->Get(Form("calo0/graphs/calibrationGraph0_%d",i));
            g->Draw("ALP");
            g->SetTitle(Form("run%s-%d, xtal%d",filename[j].substr(22,4).c_str(),stoi(filename[j].substr(22,4).c_str())+7,i));
            g->GetXaxis()->SetLabelSize(0.045);
            g->GetYaxis()->SetLabelSize(0.045);
            TF1 *pol2 = g->GetFunction("pol2");
            cout<<"photon constants: "<<pol2->GetParameter(1)<<", chi2: "<<pol2->GetChisquare()<<endl;
            constants[i]->Fill(1.0/pol2->GetParameter(1),pol2->GetChisquare());
            calibConst[i][j] = 1.0/pol2->GetParameter(1);
            calibConst1751[i] = 1.0/pol2->GetParameter(1);
        }
        //       c1->Print(Form("run%s-%d.pdf",filename[j].substr(22,4).c_str(),stoi(filename[j].substr(22,4).c_str())+7));
    }

    TCanvas *c2 = new TCanvas("c2","c2",1800,1200);
//  c2->Divide(9,6);

  //for(int i=0;i<54;i++){
 //     c2->cd(54-i);
        TGraph *trend = new TGraph(53,runNum,calibConst[15]);
       trend->SetTitle(Form("xtal%d",15));
            trend->GetXaxis()->SetLabelSize(0.04);
            trend->GetYaxis()->SetLabelSize(0.045);
        //   TGraph *trend = new TGraph(54,xtalNum,calibConst1751);
//       trend->SetMarkerStyle(20);
        trend->Draw("APL");
 // }
}
