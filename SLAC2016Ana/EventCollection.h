//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue May 24 23:45:54 2016 by ROOT version 6.06/02
// from TTree eventTree/Tree of Events
// found on file: gm2slac2016_slacAnalyzer.root
//////////////////////////////////////////////////////////

#ifndef EventCollection_h
#define EventCollection_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

using namespace std;

// Header file for the classes stored in the TTree if any.
#include <vector>


class EventCollection {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<unsigned long> *clockCounter;
   vector<unsigned int> *trigNum;
   vector<unsigned int> *Rider_EventNum;
   vector<unsigned int> *Rider_CaloNum;
   vector<unsigned int> *Rider_XtalNum;
   vector<unsigned int> *Rider_RiderNum;
   vector<unsigned int> *Rider_ChannelNum;
   vector<unsigned int> *Rider_UtcaSlotNum;
   vector<unsigned long> *Rider_ClockCounter;
   vector<unsigned int> *Rider_TriggerNum;
   vector<vector<short> > *Rider_Trace;
   vector<unsigned int> *Island_EventNum;
   vector<unsigned int> *Island_CaloNum;
   vector<unsigned int> *Island_XtalNum;
   vector<unsigned int> *Island_Num;
   vector<unsigned int> *Island_Length;
   vector<unsigned int> *Island_Time;
   vector<unsigned long> *Island_ClockCounter;
   vector<unsigned int> *Island_TriggerNum;
   vector<vector<short> > *Island_Trace;
   vector<unsigned int> *FitResult_EventNum;
   vector<unsigned int> *FitResult_CaloNum;
   vector<unsigned int> *FitResult_XtalNum;
   vector<unsigned int> *FitResult_IslandNum;
   vector<double>  *FitResult_Pedestal;
   vector<double>  *FitResult_Energy;
   vector<double>  *FitResult_Time;
   vector<double>  *FitResult_Chi2;
   vector<unsigned long> *FitResult_ClockCounter;

   // List of branches
   TBranch        *b_clockCounter;   //!
   TBranch        *b_trigNum;   //!
   TBranch        *b_Rider_EventNum;   //!
   TBranch        *b_Rider_CaloNum;   //!
   TBranch        *b_Rider_XtalNum;   //!
   TBranch        *b_Rider_RiderNum;   //!
   TBranch        *b_Rider_ChannelNum;   //!
   TBranch        *b_Rider_UtcaSlotNum;   //!
   TBranch        *b_Rider_ClockCounter;   //!
   TBranch        *b_Rider_TriggerNum;   //!
   TBranch        *b_Rider_Trace;   //!
   TBranch        *b_Island_EventNum;   //!
   TBranch        *b_Island_CaloNum;   //!
   TBranch        *b_Island_XtalNum;   //!
   TBranch        *b_Island_Num;   //!
   TBranch        *b_Island_Length;   //!
   TBranch        *b_Island_Time;   //!
   TBranch        *b_Island_ClockCounter;   //!
   TBranch        *b_Island_TriggerNum;   //!
   TBranch        *b_Island_Trace;   //!
   TBranch        *b_FitResult_EventNum;   //!
   TBranch        *b_FitResult_CaloNum;   //!
   TBranch        *b_FitResult_XtalNum;   //!
   TBranch        *b_FitResult_IslandNum;   //!
   TBranch        *b_FitResult_Pedestal;   //!
   TBranch        *b_FitResult_Energy;   //!
   TBranch        *b_FitResult_Time;   //!
   TBranch        *b_FitResult_Chi2;   //!
   TBranch        *b_FitResult_ClockCounter;   //!

   EventCollection(TTree *tree=0);
   virtual ~EventCollection();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
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
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("gm2slac2016_slacAnalyzer.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("gm2slac2016_slacAnalyzer.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("gm2slac2016_slacAnalyzer.root:/slacAnalyzer");
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
   clockCounter = 0;
   trigNum = 0;
   Rider_EventNum = 0;
   Rider_CaloNum = 0;
   Rider_XtalNum = 0;
   Rider_RiderNum = 0;
   Rider_ChannelNum = 0;
   Rider_UtcaSlotNum = 0;
   Rider_ClockCounter = 0;
   Rider_TriggerNum = 0;
   Rider_Trace = 0;
   Island_EventNum = 0;
   Island_CaloNum = 0;
   Island_XtalNum = 0;
   Island_Num = 0;
   Island_Length = 0;
   Island_Time = 0;
   Island_ClockCounter = 0;
   Island_TriggerNum = 0;
   Island_Trace = 0;
   FitResult_EventNum = 0;
   FitResult_CaloNum = 0;
   FitResult_XtalNum = 0;
   FitResult_IslandNum = 0;
   FitResult_Pedestal = 0;
   FitResult_Energy = 0;
   FitResult_Time = 0;
   FitResult_Chi2 = 0;
   FitResult_ClockCounter = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("clockCounter", &clockCounter, &b_clockCounter);
   fChain->SetBranchAddress("trigNum", &trigNum, &b_trigNum);
   fChain->SetBranchAddress("Rider_EventNum", &Rider_EventNum, &b_Rider_EventNum);
   fChain->SetBranchAddress("Rider_CaloNum", &Rider_CaloNum, &b_Rider_CaloNum);
   fChain->SetBranchAddress("Rider_XtalNum", &Rider_XtalNum, &b_Rider_XtalNum);
   fChain->SetBranchAddress("Rider_RiderNum", &Rider_RiderNum, &b_Rider_RiderNum);
   fChain->SetBranchAddress("Rider_ChannelNum", &Rider_ChannelNum, &b_Rider_ChannelNum);
   fChain->SetBranchAddress("Rider_UtcaSlotNum", &Rider_UtcaSlotNum, &b_Rider_UtcaSlotNum);
   fChain->SetBranchAddress("Rider_ClockCounter", &Rider_ClockCounter, &b_Rider_ClockCounter);
   fChain->SetBranchAddress("Rider_TriggerNum", &Rider_TriggerNum, &b_Rider_TriggerNum);
   fChain->SetBranchAddress("Rider_Trace", &Rider_Trace, &b_Rider_Trace);
   fChain->SetBranchAddress("Island_EventNum", &Island_EventNum, &b_Island_EventNum);
   fChain->SetBranchAddress("Island_CaloNum", &Island_CaloNum, &b_Island_CaloNum);
   fChain->SetBranchAddress("Island_XtalNum", &Island_XtalNum, &b_Island_XtalNum);
   fChain->SetBranchAddress("Island_Num", &Island_Num, &b_Island_Num);
   fChain->SetBranchAddress("Island_Length", &Island_Length, &b_Island_Length);
   fChain->SetBranchAddress("Island_Time", &Island_Time, &b_Island_Time);
   fChain->SetBranchAddress("Island_ClockCounter", &Island_ClockCounter, &b_Island_ClockCounter);
   fChain->SetBranchAddress("Island_TriggerNum", &Island_TriggerNum, &b_Island_TriggerNum);
   fChain->SetBranchAddress("Island_Trace", &Island_Trace, &b_Island_Trace);
   fChain->SetBranchAddress("FitResult_EventNum", &FitResult_EventNum, &b_FitResult_EventNum);
   fChain->SetBranchAddress("FitResult_CaloNum", &FitResult_CaloNum, &b_FitResult_CaloNum);
   fChain->SetBranchAddress("FitResult_XtalNum", &FitResult_XtalNum, &b_FitResult_XtalNum);
   fChain->SetBranchAddress("FitResult_IslandNum", &FitResult_IslandNum, &b_FitResult_IslandNum);
   fChain->SetBranchAddress("FitResult_Pedestal", &FitResult_Pedestal, &b_FitResult_Pedestal);
   fChain->SetBranchAddress("FitResult_Energy", &FitResult_Energy, &b_FitResult_Energy);
   fChain->SetBranchAddress("FitResult_Time", &FitResult_Time, &b_FitResult_Time);
   fChain->SetBranchAddress("FitResult_Chi2", &FitResult_Chi2, &b_FitResult_Chi2);
   fChain->SetBranchAddress("FitResult_ClockCounter", &FitResult_ClockCounter, &b_FitResult_ClockCounter);
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
