void plot_temp(){

    TCanvas *c1 = new TCanvas("c1","c1",1600,600);

    TDatime da(2016,06,01,00,00,00);
    gStyle->SetTimeOffset(da.Convert());

    // SiPM Temperature 
    TFile *file1 = new TFile("SiPMTemp.root");
    TTree *sipmTree = (TTree*)file1->Get("SiPMTemp");
    sipmTree->ls();

    char datetime1[100];
    double tsipm[54];

    sipmTree->SetBranchAddress("datetime",&datetime1);

    for(int i=0;i<54;i++){
        if(i==0 || i==9 || i==16 || i==18 || i==27 || i==36 || i==45) continue;
        sipmTree->SetBranchAddress(Form("tSiPM%d",i),&tsipm[i]);
    }

    int nentry=30000;
    float t1[nentry];
    float tempSiPM[54][nentry];
    int Y,M,D,h,m,s;

    for(int i=0;i<54;i++){
        if(i==0 || i==9 || i==16 || i==18 || i==27 || i==36 || i==45) continue;

        for(int j=0;j<nentry;j++){
            sipmTree->GetEntry(j);
            sscanf(datetime1,"%d/%d/%d %d:%d:%d",&M, &D, &Y, &h, &m, &s);
            t1[j] = s + 60*m + 3600*h + 86400*D;
            tempSiPM[i][j] = (float)tsipm[i];    
        }
    }

    TGraph *g[54];
    TGraphSmooth *gs[54];

    for(int i=0;i<54;i++){

        if(i==0 || i==9 || i==16 || i==18 || i==27 || i==36 || i==45) continue;

        g[i] = new TGraph(nentry, t1, tempSiPM[i]);
        gs[i] = new TGraphSmooth(Form("bob%d",i));
        g[i] = gs[i]->SmoothKern(g[i],"normal", 100);

        if(i==1){ 
            g[i]->Draw("AP");
            g[i]->SetMarkerColor(i);
            g[i]->GetXaxis()->SetTimeDisplay(1);
            g[i]->GetXaxis()->SetTimeFormat("%m/%d %H:%M:%S"); 
        }

        else { 
            g[i]->SetMarkerColor(i);
            g[i]->Draw("P");
        }
    }

    c1->Print("test.pdf");
}
