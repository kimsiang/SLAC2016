#include "TimeStudyAna.h"
#include <iostream>
#include <numeric> 

void TimeStudyAna::Loop(string filename){

    initialize(filename);

    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();
    Int_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        execute();

    }

    finalize(filename);

}
void TimeStudyAna::initialize(string &filename){

    //   cout << "Analyzing " << filename << ", ";

    file = new TFile(Form("%s.root",filename.c_str()),"recreate");  

    for(int i=0;i<54;i++){

        TString title0(Form("fittedSyncTime%02d",i));
        fittedSyncTime[i] = new TH1D(title0,title0,500,800,900);
        fittedSyncTime[i]->GetXaxis()->SetTitle("time [c.t.]");

        TString title1(Form("syncTime%02d",i));
        syncTime[i] = new TH1D(title1,title1,500,800,900);
        syncTime[i]->GetXaxis()->SetTitle("time [c.t.]");

        TString title2(Form("syncEnergy%02d",i));
        syncEnergy[i] = new TH1D(title2,title2,300,0,3000);
        syncEnergy[i]->GetXaxis()->SetTitle("energy [#photons]");

        for(int j=0;j<54;j++){
            TString title7(Form("syncTimeDiff%02d%02d",i,j));
            syncTimeDiff[i][j] = new TH1D(title7,title7,1000,-5,5);
        }
    }

    syncTime2D = new TH2D("syncTime2D","syncTime2D",9,-9,0,6,0,6);
    syncEnergy2D = new TH2D("syncEnergy2D","syncEnergy2D",9,-9,0,6,0,6);

    double tmp1751[54] = {0.338423,0.329669,0.212839,0.282637,0.257037,
        0.22645,0.239866,0.264617,0.281308,0.270074,
        0.274493,0.27623,0.237201,0.261981,0.318427,
        0.205304,0.279327,0.28899,0.277499,0.230175,
        0.247607,0.310288,0.263731,0.230503,0.245015,
        0.253555,0.26845,0.252464,0.331365,0.2322,
        0.217087,0.290061,0.259143,0.297331,0.247407,
        0.312086,0.242817,0.311616,0.237527,0.297094,
        0.219082,0.336318,0.251209,0.327471,0.262103,
        0.28492,0.251455,0.252973,0.244018,0.32013,
        0.270025,0.279837,0.236267,0.265446};

    double tmp2133[54] = {0.35785,0.325821,0.238763,0.27257,0.225876,0.232243,
        0.240472,0.263798,0.277963,0.272651,0.274603,0.274091,
        0.237269,0.259839,0.336021,0.21562,0.272083,0.285965,
        0.277918,0.227872,0.269181,0.274642,0.258955,0.253708,
        0.24177,0.246366,0.283048,0.230713,0.301897,0.218903,
        0.228614,0.279393,0.254203,0.303551,0.253418,0.31922,
        0.2104,0.283905,0.251689,0.273456,0.214106,0.325525,
        0.240272,0.288314,0.273695,0.274903,0.252259,0.267764,
        0.24717,0.300369,0.252639,0.27939,0.233521,0.245851};


    for(int i=0;i<54;i++){
        constants[i] = tmp1751[i];
    }
}

void TimeStudyAna::execute(){

    //      cout << "--> execute(), EventNum: " << EventNum <<endl;
    if(EventNum==99999){
        for(size_t iFit =0;iFit<54;iFit++){  
            cout<<XtalHit_Energy->at(iFit)/FitResult_Energy->at(iFit)<<","; 
        }
    }

    vector<double> syncT[54];
    vector<double> syncE[54];
    int counter[54]={};

    // Collect everything for the sync pulses
    for(size_t iFit =0;iFit<FitResult_EventNum->size();iFit++){  

        if((FitResult_Time->at(iFit) < 1000 || FitResult_Time->at(iFit) > 5000) 
                && FitResult_Chi2->at(iFit)>0 && FitResult_Energy->at(iFit)>400) {
            counter[FitResult_XtalNum->at(iFit)]++;

            if(counter[FitResult_XtalNum->at(iFit)]>100 ){
                cout<<"Event: "<<EventNum 
                    <<", XtalNum: "<<FitResult_XtalNum->at(iFit) 
                    <<", Time: "<<FitResult_Time->at(iFit) 
                    <<", E: "<<FitResult_Energy->at(iFit) 
                    <<", Chi2: "<<FitResult_Chi2->at(iFit) <<endl;
            }

            fittedSyncTime[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Time->at(iFit));
            syncT[FitResult_XtalNum->at(iFit)].push_back(FitResult_Time->at(iFit)); 
            syncE[FitResult_XtalNum->at(iFit)].push_back(constants[FitResult_XtalNum->at(iFit)]*FitResult_Energy->at(iFit)); 
            syncTime[FitResult_XtalNum->at(iFit)]->Fill(FitResult_Time->at(iFit));
            syncEnergy[FitResult_XtalNum->at(iFit)]->Fill(constants[FitResult_XtalNum->at(iFit)]*FitResult_Energy->at(iFit));
        }
    }


    for(int N=0;N<syncT[0].size();N++){
        for(int i=0;i<54;i++){
            for(int j=0;j<54;j++){
                if(syncT[i][N]!=0 && syncT[j][N]!=0)
                    syncTimeDiff[i][j]->Fill(syncT[i][N]-syncT[j][N]);
            } 
        } 
    } 

}

void TimeStudyAna::finalize(string &filename){

    const int channel_i = 0;
    const int channel_j = 9;

    double mean = syncTimeDiff[channel_i][channel_j]->GetMean();
    double rms = syncTimeDiff[channel_i][channel_j]->GetRMS();

    TF1 *fit = new TF1("fit","gaus(0)",mean-2*rms,mean+2*rms);
    fit->SetParameters(1000,mean,rms);

    syncTimeDiff[channel_i][channel_j]->Fit("fit","QMER");

    mean = fit->GetParameter(1);
    rms = fit->GetParameter(2);

    TF1 *fitd = new TF1("fitd","gaus(0)",mean-2*rms,mean+2*rms);
    fitd->SetParameters(1000,mean,rms);

    syncTimeDiff[channel_i][channel_j]->Fit("fitd","QMER");

    cout<<"dt_sync("<< channel_i<<","<< channel_j<<")[c.t.]: "<< fitd->GetParameter(1)
        <<", sigma: "<<fitd->GetParameter(2)<<endl;
    delete fit; 

    /*	if(fit->GetParameter(2)<0.1) {
        delete fit; 

        TF1 *fitd = new TF1("fitd","gaus(0)+gaus(3)",mean-2,mean+2);
        fitd->SetParameters(200,-1,0.05,200,1,0.05);

        fitd->SetParLimits(0,10,800);
        fitd->SetParLimits(1,-2,0);
        fitd->SetParLimits(2,0,0.08);
        fitd->SetParLimits(3,10,800);
        fitd->SetParLimits(4,0,2);
        fitd->SetParLimits(5,0,0.08);

        syncTimeDiff[channel_i][channel_j]->Fit("fitd","QMER");
        cout<<"mean["<< channel_i<<"]["<< channel_j<<"]: "<< fitd->GetParameter(1)
        <<", rms: "<<fitd->GetParameter(2)
        <<", 2nd mean["<< channel_i<<"]["<< channel_j<<"]: "<< fitd->GetParameter(4)
        <<", rms: "<<fitd->GetParameter(5)<<endl;
        delete fitd; 

        }
        */
    for(int i=0;i<54;i++){
        syncTime2D -> Fill (-i%9-0.5, i/9, syncTime[i]->GetMean());
        syncEnergy2D -> Fill (-i%9-0.5, i/9, syncEnergy[i]->GetMean());
    }



    //   cout << "\tfinalize(): " << filename<< endl;
    file->Write();
    file->Close();

}
