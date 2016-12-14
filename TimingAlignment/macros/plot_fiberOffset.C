void plot_fiberOffset(){

    gStyle->SetOptStat(0);

    ifstream file("fiberOffset.txt");
    ofstream outfile("fiberOffsetConstants.fcl");

    TH2D *h2 = new TH2D("fiberOffset","fiberOffset",9,0,9,6,0,6);
    double offset[54];
    int n=0;

    outfile <<"// constants from runs (xtal0 to xtal53)"<<endl;
    outfile <<"// 3295 3296 3297 3369 3300 3301 3302 3303 3368"<<endl;
    outfile <<"// 3244 3245 3246 3247 3248 3249 3250 3251 3252 "<<endl;
    outfile <<"// 3240 3239 3238 3237 3236 3235 3232 3234 3233 "<<endl;
    outfile <<"// 3267 3266 3265 3263 3258 3257 3256 3254 3253 "<<endl;
    outfile <<"// 3268 3272 3274 3275 3276 3277 3278 3281 3283 "<<endl;
    outfile <<"// 3294 3293 3292 3289 3288 3287 3286 3285 3284 "<<endl;
    outfile<< "BEGIN_PROLOG"<<endl;
    outfile<< "fiber_offset_constants: {"<<endl;

    while(file>>offset[n]){
        h2->Fill(8-n%9,n/9,offset[n]);
        outfile<<Form("xtal %d : %f",n,offset[n])<<endl;
        n++;
    }

    h2->Draw("colz");


}
