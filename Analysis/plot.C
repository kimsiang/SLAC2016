void plot(){

    gStyle->SetTitleSize(0.04,"zyx");
    gStyle->SetLabelSize(0.04,"zyx");

    // Declare file you want to analyze
    TFile *file = new TFile(Form("gm2uw_run0%d_analysis.root", 1865));

    // TTrees for headers, riders, islands, and fit results 
    TTree *header;
    TTree *rider;
    TTree *island;
    TTree *result;

    // TCanvases for headers, riders, islands, and fit results 
    TCanvas *c1 = new TCanvas("c1","Header",1200,800);
    TCanvas *c2 = new TCanvas("c2","Rider",1200,800);
    TCanvas *c3 = new TCanvas("c3","Island",1200,800);
    TCanvas *c4 = new TCanvas("c4","FitResult",1200,800);

    // TCanvas for example TCut plots
    TCanvas *c5 = new TCanvas("c5","Analysis",1200,800);

    // Get the TTrees from the TFile
    header = (TTree*)file->Get("slacAnalyzer/result/headerTree");
    rider = (TTree*)file->Get("slacAnalyzer/result/riderTree");
    island = (TTree*)file->Get("slacAnalyzer/result/islandTree");
    result = (TTree*)file->Get("slacAnalyzer/result/fitResultTree");

    // Start with the header TTree
    c1->Divide(2,2);

    // the "int" type of TH1 is only 32 bit (TH1I), which is used when you call TTree->Draw() in our case   
    // define instead TH1D (float 14 digits) to show the histogram
    TH1D *cc = new TH1D("clockCounter","clockCounter",10000,1.74e12,1.75e12);
    c1->cd(1);
    c1->cd(1)->SetLogz();
    header->Draw("clockCounter>>clockCounter"); 

    c1->cd(2);
    header->Draw("triggerNum"); 

    TH2D *cc2 = new TH2D("cctrig","clockCounter vs trigNum",200,0,1000,500,1.743e12,1.746e12);
    c1->cd(3);
    header->Draw("clockCounter:triggerNum>>cctrig","","colz"); 
    cc2->GetXaxis()->SetTitle("triggerNum");
    cc2->GetYaxis()->SetTitle("Clock Counter");


    // Now the rider TTree
    c2->Divide(3,3);
    c2->cd(1);
    rider->Draw("eventNum");

    c2->cd(2);
    rider->Draw("caloNum");

    c2->cd(3);
    rider->Draw("xtalNum");

    c2->cd(4);
    rider->Draw("riderNum");

    c2->cd(5);
    rider->Draw("riderChannelNum");

    c2->cd(6);
    rider->Draw("utcaSlotNum");

    c2->cd(7);
    rider->Draw("clockCounter");

    c2->cd(8);
    rider->Draw("triggerNum");

    c2->cd(9);
    rider->Draw("trace:Iteration$>>et(2000,0,8000,2048,-2048,2048)","","colz");

    // Now the island TTree
    c3->Divide(3,3);
    c3->cd(1);
    island->Draw("eventNum");

    c3->cd(2);
    island->Draw("caloNum");

    c3->cd(3);
    island->Draw("xtalNum");

    c3->cd(4);
    island->Draw("islandNum");

    c3->cd(5);
    island->Draw("length");

    c3->cd(6);
    island->Draw("time");

    c3->cd(7);
    island->Draw("clockCounter");

    c3->cd(8);
    island->Draw("triggerNum");

    c3->cd(9);
    island->Draw("trace:Iteration$","","colz");

    // Now the fit result TTree
    c4->Divide(3,3);
    c4->cd(1);
    result->Draw("eventNum");

    c4->cd(2);
    result->Draw("caloNum");

    c4->cd(3);
    result->Draw("xtalNum");

    c4->cd(4);
    result->Draw("islandNum");

    c4->cd(5);
    result->Draw("pedestal");

    c4->cd(6);
    result->Draw("energy");

    c4->cd(7);
    result->Draw("time");

    c4->cd(8);
    result->Draw("chi2");

    c4->cd(9);
    result->Draw("clockCounter");

    // Now example analysis plots (some with TCuts)
    c5->Divide(2,2);
    c5->cd(1);
    TCut cut0("xtalNum==20");
    island->Draw("trace:Iteration$",cut0,"colz");

    c5->cd(2);
    TCut cut1("chi2>0 && energy>0 && xtalNum==20 && pedestal > 1500");
    result->Draw("energy:clockCounter>>ec(100,1e9,3e9,50,4300,5500)",cut1,"colz");
    // example on how to access to the TH2 after creating it in TTree::Draw()
    TH2F *ec = (TH2F*)gROOT->FindObject("ec");
    ec->GetXaxis()->SetTitle("Clock Counter");
    ec->GetYaxis()->SetTitle("Energy (uncalibrated)");

    c5->cd(3);
    result->Draw("pedestal:clockCounter>>pc(100,1e9,3e9,50,1730,1750)",cut1,"colz");
    TH2F *pc = (TH2F*)gROOT->FindObject("pc");
    pc->GetXaxis()->SetTitle("Clock Counter");
    pc->GetYaxis()->SetTitle("ADC samples");

    c5->cd(4);
    result->Draw("pedestal:energy",cut1,"colz");

}

