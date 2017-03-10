void plot(){

    TFile *f[7];
    TH1D *xtal0_dt[7];
    TH2D *xtal0_syncE_dt[7];

    TCanvas *c1 = new TCanvas("c1","c1",1000,600);
    c1->Divide(2,1);

    for(int i=0;i<7;i++){

        if(i!=0 && i!=6) continue;
        f[i] = new TFile(Form("gm2slac_run0%d.root.root",1751+i));
        xtal0_dt[i] = (TH1D*)f[i]->Get("dt0");
        xtal0_syncE_dt[i] = (TH2D*)f[i]->Get("syncE_dt0");
        xtal0_dt[i]->SetLineColor(i+1);

        if(i==0){
            c1->cd(1);xtal0_dt[i]->Draw();
            c1->cd(2);xtal0_syncE_dt[i]->Draw("colz");
        }
        else {
        c1->cd(1);    xtal0_dt[i]->Draw("same"); 
        c1->cd(2);    xtal0_syncE_dt[0]->Add( xtal0_syncE_dt[i]); 
        }
    }








}
