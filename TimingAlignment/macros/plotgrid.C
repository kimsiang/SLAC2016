#include <iostream>

void plotgrid(){

    double i[1000];
    double j[1000];
    double dt[1000];
    int n=0;

    ifstream file("grid.txt");

    TCanvas *c1 = new TCanvas("c1","c1",1600,600);
    
    TH2D *grid = new TH2D("grid","grid",54,0,54,54,0,54);
    TH1D *h_dt = new TH1D("dt","dt",100,-2,2);

    while(file>>i[n]>>j[n]>>dt[n]){
	h_dt->Fill(dt[n]);
	grid->Fill(i[n],j[n],1);
	n++;
    }

    c1->Divide(2,1);
    c1->cd(1);
    c1->cd(1)->SetGridx();
    c1->cd(1)->SetGridy();
    grid->Draw("text colz");
    c1->cd(2);
    h_dt->Draw();
    c1->Print("grid.pdf");

}
