void plotFullCaloView(){

   gStyle->SetTitleW(3);
   gStyle->SetTitleH(0.08);

    TFile *file[54];
    TCanvas *canvas[54];

    TCanvas *c1= new TCanvas("c1","c1",1800,1200);

    c1->Divide(9,6);

    for(int i=0;i<54;i++){

	file[i] = new TFile(Form("event1411_xtal%d_island58.root",i));
	canvas[i] = (TCanvas*)file[i]->Get("c");
	c1->cd(54-i);
	canvas[i]->DrawClonePad();
	canvas[i]->SetTitle(Form("run2167,event1411,island58,xtal%d",i));

    }





}
