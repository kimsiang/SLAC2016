//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jul 14 16:43:34 2016 by ROOT version 5.34/25
// from TTree eventTree/Tree of Events
// found on file: /data1/slac2016/root/gm2slac_run03000_raw.root
//////////////////////////////////////////////////////////

#ifndef EventCollection_h
#define EventCollection_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

using namespace std;

// Fixed size dimensions of array or collections stored in the TTree if any.

class EventCollection {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          RunNum;
   UInt_t          EventNum;
   vector<unsigned long long> *CaloAMC13_ClockCounter;
   vector<unsigned int> *CaloAMC13_TriggerNum;
   vector<unsigned long long> *FC7_ClockCounter;
   vector<unsigned int> *FC7_TriggerNum;
   vector<unsigned int> *FitResult_EventNum;
   vector<unsigned int> *FitResult_CaloNum;
   vector<unsigned int> *FitResult_XtalNum;
   vector<unsigned int> *FitResult_UtcaSlotNum;
   vector<unsigned int> *FitResult_ChannelNum;
   vector<unsigned int> *FitResult_IslandNum;
   vector<double>  *FitResult_Pedestal;
   vector<double>  *FitResult_Energy;
   vector<double>  *FitResult_Time;
   vector<double>  *FitResult_Chi2;
   vector<unsigned long long> *FitResult_ClockCounter;
   vector<unsigned int> *Italiano_EventNum;
   vector<unsigned int> *Italiano_CaloNum;
   vector<unsigned int> *Italiano_XtalNum;
   vector<unsigned int> *Italiano_UtcaSlotNum;
   vector<unsigned int> *Italiano_ChannelNum;
   vector<unsigned int> *Italiano_IslandNum;
   vector<double>  *Italiano_Pedestal;
   vector<double>  *Italiano_Amplitude;
   vector<double>  *Italiano_Area;
   vector<double>  *Italiano_Time;
   vector<unsigned long long> *Italiano_ClockCounter;
   vector<unsigned int> *XtalHit_EventNum;
   vector<unsigned int> *XtalHit_CaloNum;
   vector<unsigned int> *XtalHit_XtalNum;
   vector<unsigned int> *XtalHit_UtcaSlotNum;
   vector<unsigned int> *XtalHit_ChannelNum;
   vector<unsigned int> *XtalHit_IslandNum;
   vector<double>  *XtalHit_Energy;
   vector<double>  *XtalHit_Time;
   vector<double>  *XtalHit_ClockCounter;
   vector<unsigned int> *Cluster_EventNum;
   vector<unsigned int> *Cluster_CaloNum;
   vector<unsigned int> *Cluster_IslandNum;
   vector<double>  *Cluster_Energy;
   vector<double>  *Cluster_Time;
   vector<double>  *Cluster_X;
   vector<double>  *Cluster_Y;

   // List of branches
   TBranch        *b_RunNum;   //!
   TBranch        *b_EventNum;   //!
   TBranch        *b_CaloAMC13_ClockCounter;   //!
   TBranch        *b_CaloAMC13_TriggerNum;   //!
   TBranch        *b_FC7_ClockCounter;   //!
   TBranch        *b_FC7_TriggerNum;   //!
   TBranch        *b_FitResult_EventNum;   //!
   TBranch        *b_FitResult_CaloNum;   //!
   TBranch        *b_FitResult_XtalNum;   //!
   TBranch        *b_FitResult_UtcaSlotNum;   //!
   TBranch        *b_FitResult_ChannelNum;   //!
   TBranch        *b_FitResult_IslandNum;   //!
   TBranch        *b_FitResult_Pedestal;   //!
   TBranch        *b_FitResult_Energy;   //!
   TBranch        *b_FitResult_Time;   //!
   TBranch        *b_FitResult_Chi2;   //!
   TBranch        *b_FitResult_ClockCounter;   //!
   TBranch        *b_Italiano_EventNum;   //!
   TBranch        *b_Italiano_CaloNum;   //!
   TBranch        *b_Italiano_XtalNum;   //!
   TBranch        *b_Italiano_UtcaSlotNum;   //!
   TBranch        *b_Italiano_ChannelNum;   //!
   TBranch        *b_Italiano_IslandNum;   //!
   TBranch        *b_Italiano_Pedestal;   //!
   TBranch        *b_Italiano_Amplitude;   //!
   TBranch        *b_Italiano_Area;   //!
   TBranch        *b_Italiano_Time;   //!
   TBranch        *b_Italiano_ClockCounter;   //!
   TBranch        *b_XtalHit_EventNum;   //!
   TBranch        *b_XtalHit_CaloNum;   //!
   TBranch        *b_XtalHit_XtalNum;   //!
   TBranch        *b_XtalHit_UtcaSlotNum;   //!
   TBranch        *b_XtalHit_ChannelNum;   //!
   TBranch        *b_XtalHit_IslandNum;   //!
   TBranch        *b_XtalHit_Energy;   //!
   TBranch        *b_XtalHit_Time;   //!
   TBranch        *b_XtalHit_ClockCounter;   //!
   TBranch        *b_Cluster_EventNum;   //!
   TBranch        *b_Cluster_CaloNum;   //!
   TBranch        *b_Cluster_IslandNum;   //!
   TBranch        *b_Cluster_Energy;   //!
   TBranch        *b_Cluster_Time;   //!
   TBranch        *b_Cluster_X;   //!
   TBranch        *b_Cluster_Y;   //!

   EventCollection(TTree *tree=0);
   virtual ~EventCollection();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(std::string filename);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef EventCollection_cxx
EventCollection::EventCollection(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/data1/slac2016/root/gm2slac_run03000_raw.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/data1/slac2016/root/gm2slac_run03000_raw.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("/data1/slac2016/root/gm2slac_run03000_raw.root:/slacAnalyzer");
      dir->GetObject("eventTree",tree);

   }
   Init(tree);
}

EventCollection::~EventCollection()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t EventCollection::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t EventCollection::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void EventCollection::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   CaloAMC13_ClockCounter = 0;
   CaloAMC13_TriggerNum = 0;
   FC7_ClockCounter = 0;
   FC7_TriggerNum = 0;
   FitResult_EventNum = 0;
   FitResult_CaloNum = 0;
   FitResult_XtalNum = 0;
   FitResult_UtcaSlotNum = 0;
   FitResult_ChannelNum = 0;
   FitResult_IslandNum = 0;
   FitResult_Pedestal = 0;
   FitResult_Energy = 0;
   FitResult_Time = 0;
   FitResult_Chi2 = 0;
   FitResult_ClockCounter = 0;
   Italiano_EventNum = 0;
   Italiano_CaloNum = 0;
   Italiano_XtalNum = 0;
   Italiano_UtcaSlotNum = 0;
   Italiano_ChannelNum = 0;
   Italiano_IslandNum = 0;
   Italiano_Pedestal = 0;
   Italiano_Amplitude = 0;
   Italiano_Area = 0;
   Italiano_Time = 0;
   Italiano_ClockCounter = 0;
   XtalHit_EventNum = 0;
   XtalHit_CaloNum = 0;
   XtalHit_XtalNum = 0;
   XtalHit_UtcaSlotNum = 0;
   XtalHit_ChannelNum = 0;
   XtalHit_IslandNum = 0;
   XtalHit_Energy = 0;
   XtalHit_Time = 0;
   XtalHit_ClockCounter = 0;
   Cluster_EventNum = 0;
   Cluster_CaloNum = 0;
   Cluster_IslandNum = 0;
   Cluster_Energy = 0;
   Cluster_Time = 0;
   Cluster_X = 0;
   Cluster_Y = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNum", &RunNum, &b_RunNum);
   fChain->SetBranchAddress("EventNum", &EventNum, &b_EventNum);
   fChain->SetBranchAddress("CaloAMC13_ClockCounter", &CaloAMC13_ClockCounter, &b_CaloAMC13_ClockCounter);
   fChain->SetBranchAddress("CaloAMC13_TriggerNum", &CaloAMC13_TriggerNum, &b_CaloAMC13_TriggerNum);
   fChain->SetBranchAddress("FC7_ClockCounter", &FC7_ClockCounter, &b_FC7_ClockCounter);
   fChain->SetBranchAddress("FC7_TriggerNum", &FC7_TriggerNum, &b_FC7_TriggerNum);
   fChain->SetBranchAddress("FitResult_EventNum", &FitResult_EventNum, &b_FitResult_EventNum);
   fChain->SetBranchAddress("FitResult_CaloNum", &FitResult_CaloNum, &b_FitResult_CaloNum);
   fChain->SetBranchAddress("FitResult_XtalNum", &FitResult_XtalNum, &b_FitResult_XtalNum);
   fChain->SetBranchAddress("FitResult_UtcaSlotNum", &FitResult_UtcaSlotNum, &b_FitResult_UtcaSlotNum);
   fChain->SetBranchAddress("FitResult_ChannelNum", &FitResult_ChannelNum, &b_FitResult_ChannelNum);
   fChain->SetBranchAddress("FitResult_IslandNum", &FitResult_IslandNum, &b_FitResult_IslandNum);
   fChain->SetBranchAddress("FitResult_Pedestal", &FitResult_Pedestal, &b_FitResult_Pedestal);
   fChain->SetBranchAddress("FitResult_Energy", &FitResult_Energy, &b_FitResult_Energy);
   fChain->SetBranchAddress("FitResult_Time", &FitResult_Time, &b_FitResult_Time);
   fChain->SetBranchAddress("FitResult_Chi2", &FitResult_Chi2, &b_FitResult_Chi2);
   fChain->SetBranchAddress("FitResult_ClockCounter", &FitResult_ClockCounter, &b_FitResult_ClockCounter);
   fChain->SetBranchAddress("Italiano_EventNum", &Italiano_EventNum, &b_Italiano_EventNum);
   fChain->SetBranchAddress("Italiano_CaloNum", &Italiano_CaloNum, &b_Italiano_CaloNum);
   fChain->SetBranchAddress("Italiano_XtalNum", &Italiano_XtalNum, &b_Italiano_XtalNum);
   fChain->SetBranchAddress("Italiano_UtcaSlotNum", &Italiano_UtcaSlotNum, &b_Italiano_UtcaSlotNum);
   fChain->SetBranchAddress("Italiano_ChannelNum", &Italiano_ChannelNum, &b_Italiano_ChannelNum);
   fChain->SetBranchAddress("Italiano_IslandNum", &Italiano_IslandNum, &b_Italiano_IslandNum);
   fChain->SetBranchAddress("Italiano_Pedestal", &Italiano_Pedestal, &b_Italiano_Pedestal);
   fChain->SetBranchAddress("Italiano_Amplitude", &Italiano_Amplitude, &b_Italiano_Amplitude);
   fChain->SetBranchAddress("Italiano_Area", &Italiano_Area, &b_Italiano_Area);
   fChain->SetBranchAddress("Italiano_Time", &Italiano_Time, &b_Italiano_Time);
   fChain->SetBranchAddress("Italiano_ClockCounter", &Italiano_ClockCounter, &b_Italiano_ClockCounter);
   fChain->SetBranchAddress("XtalHit_EventNum", &XtalHit_EventNum, &b_XtalHit_EventNum);
   fChain->SetBranchAddress("XtalHit_CaloNum", &XtalHit_CaloNum, &b_XtalHit_CaloNum);
   fChain->SetBranchAddress("XtalHit_XtalNum", &XtalHit_XtalNum, &b_XtalHit_XtalNum);
   fChain->SetBranchAddress("XtalHit_UtcaSlotNum", &XtalHit_UtcaSlotNum, &b_XtalHit_UtcaSlotNum);
   fChain->SetBranchAddress("XtalHit_ChannelNum", &XtalHit_ChannelNum, &b_XtalHit_ChannelNum);
   fChain->SetBranchAddress("XtalHit_IslandNum", &XtalHit_IslandNum, &b_XtalHit_IslandNum);
   fChain->SetBranchAddress("XtalHit_Energy", &XtalHit_Energy, &b_XtalHit_Energy);
   fChain->SetBranchAddress("XtalHit_Time", &XtalHit_Time, &b_XtalHit_Time);
   fChain->SetBranchAddress("XtalHit_ClockCounter", &XtalHit_ClockCounter, &b_XtalHit_ClockCounter);
   fChain->SetBranchAddress("Cluster_EventNum", &Cluster_EventNum, &b_Cluster_EventNum);
   fChain->SetBranchAddress("Cluster_CaloNum", &Cluster_CaloNum, &b_Cluster_CaloNum);
   fChain->SetBranchAddress("Cluster_IslandNum", &Cluster_IslandNum, &b_Cluster_IslandNum);
   fChain->SetBranchAddress("Cluster_Energy", &Cluster_Energy, &b_Cluster_Energy);
   fChain->SetBranchAddress("Cluster_Time", &Cluster_Time, &b_Cluster_Time);
   fChain->SetBranchAddress("Cluster_X", &Cluster_X, &b_Cluster_X);
   fChain->SetBranchAddress("Cluster_Y", &Cluster_Y, &b_Cluster_Y);
   Notify();
}

Bool_t EventCollection::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void EventCollection::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t EventCollection::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef EventCollection_cxx
