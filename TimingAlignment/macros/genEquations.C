void genEquations(){


ifstream infile("grid.txt");

int i[238],j[238];
double t[238], rms[238];
int n=0;

while(infile>>i[n]>>j[n]>>t[n]>>rms[n]){
//cout<<fixed<<setprecision(1)<<"a"<<i[n]<<" - a"<<j[n]<<" == "<<t[n]<<", ";
cout<<"a"<<i[n]<<" - a"<<j[n]<<" == "<<t[n]<<", ";
if(abs(t[n])<0.01) cout<<"#"<<t[n]<<endl;
n++;

}



cout<<"{";
for(int i=0;i<54;i++){
cout<<"a"<<i<<",";
}
cout<<"}"<<endl;

}
