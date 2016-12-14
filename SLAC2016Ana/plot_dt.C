void plot_dt(){

    gStyle->SetOptStat(0010);
    gStyle->SetOptFit(0011);
    gStyle->SetTitleH(0.1);
    gStyle->SetTitleW(2);
    gStyle->SetPalette(55);

    int runnumber = 3238;
/*    int runlist[54] = {3295,3296,3297,3298,3300,3301,3302,3303,3304,
	3244,3245,3246,3247,3248,3249,3250,3251,3252,
	3240,3239,3238,3237,3236,3235,3232,3234,3233,
	3267,3266,3265,3263,3258,3257,3256,3254,3253,
	3268,3272,3274,3275,3276,3277,3278,3281,3283,
	3294,3293,3292,3289,3288,3287,3286,3285,3284};
*/
    int runlist[54] = {3295,3296,3297,3369,3300,3301,3302,3303,3368,
	3244,3245,3246,3247,3248,3249,3250,3251,3252,
	3240,3239,3238,3237,3236,3235,3232,3234,3233,
	3267,3266,3265,3263,3258,3257,3256,3254,3253,
	3268,3272,3274,3275,3276,3277,3278,3281,3283,
	3294,3293,3292,3289,3288,3287,3286,3285,3284};


    TH1D *beamTime[6];
    TH2D *beamTimeEnergy[6];


    // for(int iter = 0; iter< sizeof(runlist)/sizeof(runlist[0]);iter++){
    for(int iter = 0; iter< 54;iter++){
//if(iter==9 || iter==16 || iter==18) continue;
	if(iter%9==0 || iter%9==8) continue;
	if(iter<9 || iter>44) continue;

	runnumber = runlist[iter] ;      
//runnumber = 1935;      

	cout<<"["<<iter<<"]"<<"Analyzing run"<<runnumber<<endl;

	TFile *file = new TFile(Form("run%04d.root",runnumber));
	//       file = new TFile("testrun.root");
	ofstream gridfile("grid.txt", ios::app);

	TCanvas *c1 = new TCanvas("c1","c1",900,600);

	TH2D *beamEnergy2D = (TH2D*)file->Get("beamEnergy2D");
	beamEnergy2D->Draw("colz text");
	beamEnergy2D->SetMarkerSize(1);

	double maxE = beamEnergy2D->GetMaximum();
	int maxBin = beamEnergy2D->GetMaximumBin();
	int x,y,z;
	beamEnergy2D->GetBinXYZ(maxBin,x,y,z);
	cout<<x<<" "<<y<<" "<<z<<" "<<(y-1)*9+9-x<<endl;
	int xtalhit = (y-1)*9+9-x; 

	TCanvas *c2 = new TCanvas("c2","c2",800,600);
	//       c2->Divide(3,2);


	beamTime[0] = (TH1D*)file->Get(Form("beamTimeDiff%02d%02d",xtalhit-9,xtalhit-1));
	beamTime[1] = (TH1D*)file->Get(Form("beamTimeDiff%02d%02d",xtalhit-9,xtalhit+1));
	beamTime[2] = (TH1D*)file->Get(Form("beamTimeDiff%02d%02d",xtalhit-9,xtalhit+9));
	beamTime[3] = (TH1D*)file->Get(Form("beamTimeDiff%02d%02d",xtalhit-1,xtalhit+1));
	beamTime[4] = (TH1D*)file->Get(Form("beamTimeDiff%02d%02d",xtalhit-1,xtalhit+9));
	beamTime[5] = (TH1D*)file->Get(Form("beamTimeDiff%02d%02d",xtalhit+1,xtalhit+9));

	beamTimeEnergy[0] = (TH2D*)file->Get(Form("beamTimeDiffEnergy%02d%02d",xtalhit-9,xtalhit-1));
	beamTimeEnergy[1] = (TH2D*)file->Get(Form("beamTimeDiffEnergy%02d%02d",xtalhit-9,xtalhit+1));
	beamTimeEnergy[2] = (TH2D*)file->Get(Form("beamTimeDiffEnergy%02d%02d",xtalhit-9,xtalhit+9));
	beamTimeEnergy[3] = (TH2D*)file->Get(Form("beamTimeDiffEnergy%02d%02d",xtalhit-1,xtalhit+1));
	beamTimeEnergy[4] = (TH2D*)file->Get(Form("beamTimeDiffEnergy%02d%02d",xtalhit-1,xtalhit+9));
	beamTimeEnergy[5] = (TH2D*)file->Get(Form("beamTimeDiffEnergy%02d%02d",xtalhit+1,xtalhit+9));

	for(int i=0;i<6;i++){

	    c2->cd();

	    if(!beamTime[i]) continue;
	    //           beamTime[i]->Rebin(2);
	    beamTime[i]->Draw("E");
	    beamTime[i]->SetLineColor(1);
	    beamTime[i]->SetTitleSize(0.05,"z");

	    int binmax = beamTime[i]->GetMaximumBin();
	    double xmax = beamTime[i]->GetXaxis()->GetBinCenter(binmax);

	    double mean = beamTime[i]->GetMean();
	    double rms = beamTime[i]->GetRMS();

	    TF1 *fit = new TF1("fit","gaus(0)", mean - 2.0*rms, mean + 2.0*rms);
	    fit->SetParameters(20,mean,rms);
	    fit->SetParLimits(0,10,1000);
	    fit->SetParLimits(1,-2,2);
	    fit->SetParLimits(2,0.05,0.3);
	    beamTime[i]->Fit("fit","QMER");
	    double norm = fit->GetParameter(0);
	    mean = fit->GetParameter(1);
	    rms = fit->GetParameter(2);

	    TF1 *fit2 = new TF1("fit2","gaus(0)", mean - 2.0*rms, mean + 2.0*rms);
	    fit2->SetParameters(norm,mean,rms);
	    beamTime[i]->Fit("fit2","QMER");
	    mean = fit2->GetParameter(1);
	    rms = fit2->GetParameter(2);

	    beamTime[i]->GetXaxis()->SetRangeUser(mean - 4.0*rms, mean + 4.0*rms);
	    beamTime[i]->GetXaxis()->SetTitle("dt[c.t.]");


	    gPad->Update();
	    TPaveStats *ps = (TPaveStats*)c2->GetPrimitive("stats");
	    ps->SetName("stat2"); 
	    ps->SetX1NDC(0.6); 
	    ps->SetX2NDC(0.9); 
	    ps->SetY1NDC(0.7); 
	    ps->SetY2NDC(0.9); 
	    c2->Print(Form("run%d_%s.pdf",runnumber,beamTime[i]->GetTitle()));
	    //  beamTimeEnergy[i]->Rebin2D(1);
	    //  beamTimeEnergy[i]->Draw("colz");
	    // beamTimeEnergy[i]->GetXaxis()->SetTitle("t_{23} - t_{25} [c.t.]");
	    //beamTimeEnergy[i]->GetYaxis()->SetTitle("E_{23} - E_{25} [#photon]");
	    //           beamTimeEnergy[i]->Fit("pol1");

	    if(i==0) { gridfile<<xtalhit-9<<" "<<xtalhit-1<<" "<<mean<<endl;}
	    if(i==1) { gridfile<<xtalhit-9<<" "<<xtalhit+1<<" "<<mean<<endl;}
	    if(i==2) { gridfile<<xtalhit-9<<" "<<xtalhit+9<<" "<<mean<<endl;}
	    if(i==3) { gridfile<<xtalhit-1<<" "<<xtalhit+1<<" "<<mean<<endl;}
	    if(i==4) { gridfile<<xtalhit-1<<" "<<xtalhit+9<<" "<<mean<<endl;}
	    if(i==5) { gridfile<<xtalhit+1<<" "<<xtalhit+9<<" "<<mean<<endl;}
	}

	TCanvas *c3 = new TCanvas("c3","c3",800,600);
	beamTime[3]->Draw();
	// Retrieve the stat box


    }

    /*
       beamTimeEnergy[3]->GetXaxis()->SetRangeUser(-2,2);
       beamTimeEnergy[3]->SetTitle("#DeltaE vs #Deltat");
       beamTimeEnergy[3]->Draw("colz");
       */
}

