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
    c1->Divide(9,6);

    for(int j=0;j<53;j++){

        for(int i=0;i<54;i++){
            c1->cd(54-i);
            TGraph *g = (TGraph*)f[j]->Get(Form("calo0/graphs/calibrationGraph0_%d",i));
            g->Draw("ALP");
            g->SetTitle(Form("run%s-%d, xtal%d",filename[j].substr(22,4).c_str(),stoi(filename[j].substr(22,4).c_str())+7,i));
            g->GetXaxis()->SetLabelSize(0.045);
            g->GetYaxis()->SetLabelSize(0.045);
        }

        c1->Print(Form("run%s-%d.pdf",filename[j].substr(22,4).c_str(),stoi(filename[j].substr(22,4).c_str())+7));
    }
}
