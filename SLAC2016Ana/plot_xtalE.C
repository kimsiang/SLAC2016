void plot_xtalE(){

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetTitleSize(0.08,"t");

    // a list of runs corresponding to xtal0 to xtal53
    int runlist[54] = {3295,3296,3297,3369,3300,3301,3302,3303,3368,
	3244,3245,3246,3247,3248,3249,3250,3251,3252,
	3240,3239,3238,3237,3236,3235,3232,3234,3233,
	3267,3266,3265,3263,3258,3257,3256,3254,3253,
	3268,3272,3274,3275,3276,3277,3278,3281,3283,
	3294,3293,3292,3289,3288,3287,3286,3285,3284};

    //    for(int i=0;i<54;i++){
    //	runlist[i] = 3305;
    //   }

    ofstream outfile("energycalibration.fcl");
    TCanvas *c1 = new TCanvas("c1","c1",1800,1200);
    TH2D *xtalEnergy = new TH2D("xtalEnergy","xtalEnergy",9,0,9,6,0,6);

    outfile << "// constants from runs" <<endl;;

    for(int i=0;i<54;i++){
	outfile <<  runlist[i]  <<  " ";
	if(i%9==8 && i!=53) {
	    outfile << endl;
	    outfile << "// ";}
    } 

    outfile << "\nBEGIN_PROLOG" << endl;
    outfile << "xtal_energy_response: {" << endl;

    double mean[54];
    double rms[54];
    double norm[54];
    double index[54];

    c1->Divide(9,6);

    for(int i=0;i<54;i++){
	index[i]=i;
	c1->cd(54-i);
	c1->cd(54-i)->SetLogz();

	//if(i==3 || i==8 || i==9) continue; // these are runs where the beams are at the wrong place

	TFile *file = new TFile(Form("run%d.root",runlist[i]));

	TH2D *beamEnergy2D = (TH2D*)file->Get("beamEnergy2D");
	beamEnergy2D->Draw("colz text");
	beamEnergy2D->SetMarkerSize(1);
	beamEnergy2D->SetMaximum(3000);
	beamEnergy2D->SetMinimum(1);
	beamEnergy2D->SetTitle(Form("Run %d",runlist[i]));


	double maxE = beamEnergy2D->GetMaximum();
	int maxBin = beamEnergy2D->GetMaximumBin();
	int x,y,z;
	beamEnergy2D->GetBinXYZ(maxBin,x,y,z);
	cout<<x<<" "<<y<<" "<<z<<" "<<(y-1)*9+9-x<<endl;
	int xtalNum = (y-1)*9+9-x; 

	TH1D *beamEnergy = (TH1D*)file->Get(Form("beamEnergy%02d",xtalNum));
	TH1D *beamTime = (TH1D*)file->Get(Form("beamTime%02d",xtalNum));
	TH1D *beamTimeEnergy = (TH1D*)file->Get(Form("beamTimeEnergy%02d",xtalNum));
	TH1D *syncEnergy = (TH1D*)file->Get(Form("syncEnergy%02d",xtalNum));

	beamEnergy->Draw();
	TF1 *fit = new TF1("fit","gaus(0)",1500,2800);
	fit->SetParLimits(1,1700,2500);
	fit->SetParLimits(2,50,150);
	fit->SetParameters(100,2100,100);
	beamEnergy->Fit("fit","REM");


	norm[i]=fit->GetParameter(0);    
	mean[i]=fit->GetParameter(1);    
	rms[i]=fit->GetParameter(2);    

	TF1 *refit = new TF1("refit","gaus(0)",mean[i]-2*rms[i], mean[i]+2*rms[i]);
	refit->SetParameters(norm[i],mean[i],rms[i]);
	beamEnergy->Fit("refit","REM");

	norm[i]=refit->GetParameter(0);    
	mean[i]=refit->GetParameter(1);    
	rms[i]=refit->GetParameter(2);    

        outfile << Form("xtal%d : %f",i,mean[i]) << endl;

	xtalEnergy->Fill(8-i%9,i/9,mean[i]);

	TText *text = new TText(0.15,0.75,Form("E%d=%.1f",xtalNum,mean[i]));
	text->SetTextSize(0.09);
	text->SetTextColor(2);
	text->SetNDC();
	beamEnergy->GetListOfFunctions()->Add(text);
    }


    outfile << "}" << endl;
    outfile << "END_PROLOG" << endl;

    TCanvas *c2 = new TCanvas("c2","c2",900,600);
    xtalEnergy->Draw("colz text");
    xtalEnergy->SetMarkerSize(1.5);
    xtalEnergy->SetMaximum(2500);
    xtalEnergy->SetMinimum(1700);

    TCanvas *c3 = new TCanvas("c3","c3",900,600);
    TGraph *g1 = new TGraph(54,index,mean);
    g1->SetMarkerStyle(20);
    g1->Draw("AP");

    double avg=0;

    for(int i=0;i<54;i++){
	if(mean[i]>0){
	    avg += mean[i]/54.;
	}
    }

    cout<<avg<<endl;

    TLine *linem5 = new TLine(0,0.95*avg,54,0.95*avg);
    TLine *linep5 = new TLine(0,1.05*avg,54,1.05*avg);
    linem5->SetLineWidth(2);
    linem5->SetLineStyle(7);
    linem5->SetLineColor(2);
    linep5->SetLineWidth(2);
    linep5->SetLineColor(2);
    linep5->SetLineStyle(7);
    linem5->Draw("same");
    linep5->Draw("same");

    TLine *line1 = new TLine(0,0.9*avg,54,0.9*avg);
    line1->SetLineWidth(2);
    line1->SetLineColor(4);
    line1->Draw("same");


    TLine *line2 = new TLine(0,1.1*avg,54,1.1*avg);
    line2->SetLineWidth(2);
    line2->SetLineColor(4);
    line2->Draw("same");

    TLegend *leg = new TLegend(0.7,0.1,0.9,0.3);
    leg->AddEntry(linem5,"#pm 5%","l");
    leg->AddEntry(line1,"#pm 10%","l");
    leg->Draw();

}
