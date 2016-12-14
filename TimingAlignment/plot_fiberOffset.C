void plot_fiberOffset(){

    gStyle->SetOptStat(0);

    ifstream file("fiberOffset.txt");

    TH2D *h2 = new TH2D("fiberOffset","fiberOffset",9,0,9,6,0,6);
    double offset[54];
    int n=0;

    while(file>>offset[n]){
        h2->Fill(8-n%9,n/9,offset[n]);
        n++;
    }

    h2->Draw("colz");


}
