/*
  Aaron Fienberg
  fienberg@uw.edu
  code for generating "fuzzy templates" based on digitized datasets from slac
  analyzer
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "TSystem.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TSpline.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TString.h"
#include "time.h"
using namespace std;

const bool POSITIVE = false;

const int TEMPLATELENGTH = 100;
const int NBINSPSEUDOTIME = 500;
const int NTIMEBINS = 10;
const int BASELINEFITLENGTH = 20;
const int BUFFERZONE = 20;

typedef struct traceSummary {
  double pseudoTime;
  int peakIndex;
  double baseline;
  double integral;
  double normalizedAmpl;
  bool bad;
} traceSummary;

traceSummary processTrace(const vector<short>& trace);
vector<double> correctTrace(const vector<short>& trace, traceSummary summary);

int main(int argc, const char* argv[]) {
  clock_t t1, t2;
  t1 = clock();

  if (argc < 3) {
    cout << "need input file and xtal num" << endl;
    exit(0);
  }

  unsigned int chosenXtalNum = atoi(argv[2]);

  // read input file
  gSystem->Load("libTree");
  TFile infile(argv[1]);
  // unique_ptr<TTree> t((TTree*)infile.Get("t"));
  unique_ptr<TTree> t((TTree*)infile.Get("slacAnalyzer/islandTree"));

  vector<short>* tracevec = 0;
  unsigned int xtalNum = 0;

  t->SetBranchAddress("Trace", &tracevec);
  t->SetBranchAddress("XtalNum", &xtalNum);

  // process traces
  cout << "Processing traces... " << endl;
  vector<traceSummary> summaries(t->GetEntries());
  TH1D pseudoTimesHist("ptimes", "ptimes", NBINSPSEUDOTIME, 0, 1);
  TH1D normalizedMaxes("maxes", "maxes", 100, 0.0, 0.0);
  for (int i = 0; i < t->GetEntries(); ++i) {
    t->GetEntry(i);
    // cout << tracevec->size() << endl;
    // cin.ignore();
    // // cout << "trace 0 " << tracevec->at(0) << endl;
    // // cout << "xtal num " << xtalNum << endl;
    // // exit(0);
    if (xtalNum != chosenXtalNum) {
      summaries[i].bad = true;
    } else {
      summaries[i] = processTrace(*tracevec);
      pseudoTimesHist.Fill(summaries[i].pseudoTime);
      normalizedMaxes.Fill(summaries[i].normalizedAmpl);
    }
    if (i % 1000 == 0) {
      cout << "Trace " << i << " processed." << endl;
    }
  }

  pseudoTimesHist.Scale(1.0 / pseudoTimesHist.Integral());

  // find max for fuzzy template bin range
  normalizedMaxes.Fit("gaus", "q0");
  double binRangeMax = normalizedMaxes.GetFunction("gaus")->GetParameter(1) +
                       5 * normalizedMaxes.GetFunction("gaus")->GetParameter(2);

  // create map to real time
  TGraph realTimes(0);
  realTimes.SetName("realTimeGraph");
  realTimes.SetPoint(0, 0, 0);
  for (int i = 0; i < NBINSPSEUDOTIME; ++i) {
    realTimes.SetPoint(i, pseudoTimesHist.GetBinLowEdge(i + 2),
                       pseudoTimesHist.Integral(1, i + 1));
  }
  TSpline3 rtSpline = TSpline3("realTimeSpline", &realTimes);
  rtSpline.SetName("realTimeSpline");

  // fill the timeslices and make the master fuzzy template
  TH2D masterFuzzyTemplate =
      TH2D("masterFuzzy", "Fuzzy Template", TEMPLATELENGTH * NTIMEBINS,
           -.5 - BUFFERZONE, TEMPLATELENGTH - .5 - BUFFERZONE, 1000,
           -.2 * binRangeMax, binRangeMax);

  cout << "Populating timeslices... " << endl;
  for (int i = 0; i < t->GetEntries(); ++i) {
    t->GetEntry(i);
    if (summaries[i].bad) {
      continue;
    }
    double realTime = rtSpline.Eval(summaries[i].pseudoTime);
    int thisSlice = static_cast<int>(realTime * NTIMEBINS);
    if (thisSlice == NTIMEBINS) {
      --thisSlice;
    }
    auto ctrace = correctTrace(*tracevec, summaries[i]);
    for (int j = 0; j < TEMPLATELENGTH; ++j) {
      masterFuzzyTemplate.Fill(j - realTime + 0.5 - BUFFERZONE, ctrace[j]);
    }
    if (i % 1000 == 0) {
      cout << "Trace " << i << " placed." << endl;
    }
  }

  // step through fuzzy template to get errors and means
  cout << "Calculating errors and means... " << endl;
  TGraphErrors masterGraph(0);
  masterGraph.SetName("masterGraph");
  for (int i = 0; i < TEMPLATELENGTH * NTIMEBINS; ++i) {
    unique_ptr<TH1D> xBinHist(
        masterFuzzyTemplate.ProjectionY("binhist", i + 1, i + 1));
    xBinHist->Fit("gaus", "q0", "",
                  xBinHist->GetMean() - xBinHist->GetRMS() * 3,
                  xBinHist->GetMean() + xBinHist->GetRMS() * 3);
    double mean = xBinHist->GetFunction("gaus")->GetParameter(1);
    double sig = xBinHist->GetFunction("gaus")->GetParameter(2);
    masterGraph.SetPoint(
        i, static_cast<double>(i) / NTIMEBINS - BUFFERZONE - .5, mean);
    masterGraph.SetPointError(i, 0, sig);
  }
  cout << "Errors and Means Calculated" << endl;

  TSpline3 masterSpline("masterSpline", &masterGraph);
  masterSpline.SetName("masterSpline");
  masterSpline.SetNpx(10000);

  // save data
  TFile outf(Form("laserTemplateFile%i.root", chosenXtalNum), "recreate");
  rtSpline.Write();
  pseudoTimesHist.Write();
  masterFuzzyTemplate.Write();
  masterGraph.Write();
  masterSpline.Write();
  outf.Write();
  outf.Close();

  // finish up
  t2 = clock();
  double diff((double)t2 - (double)t1);
  cout << "Time elapsed: " << diff / CLOCKS_PER_SEC << " seconds." << endl;
  return 0;
}

traceSummary processTrace(const vector<short>& trace) {
  traceSummary results;
  results.bad = false;

  // find maximum or minimum
  int maxdex = 0;
  for (std::size_t i = 0; i < trace.size(); ++i) {
    if (POSITIVE) {
      maxdex = trace[i] > trace[maxdex] ? i : maxdex;
    } else {
      maxdex = trace[i] < trace[maxdex] ? i : maxdex;
    }
  }
  results.peakIndex = maxdex;
  // std::cout << maxdex << std::endl;
  // std::cout << trace[maxdex] << std::endl;
  // std::cin.ignore();

  // if (maxdex < 50 || maxdex > 60) {
  //   results.bad = true;
  //   return results;
  // if (trace[maxdex] > 500) {
  //   results.bad = true;
  //   return results;
  // }

  // calculate pseudotime
  if (trace[maxdex] == trace[maxdex + 1])
    results.pseudoTime = 1;
  else {
    results.pseudoTime =
        2.0 / M_PI *
        atan(static_cast<double>(trace[maxdex - 1] - trace[maxdex]) /
             (trace[maxdex + 1] - trace[maxdex]));
  }

  if (results.pseudoTime == 0) {
    cin.ignore();
  }

  // get the baseline
  if (maxdex - BASELINEFITLENGTH - BUFFERZONE < 0) {
    cout << "Baseline fit walked off the end of the trace!" << endl;
    results.bad = true;
    return results;
  }
  double runningBaseline = 0;
  for (int i = 0; i < BASELINEFITLENGTH; ++i) {
    runningBaseline =
        runningBaseline + trace[maxdex - BUFFERZONE - BASELINEFITLENGTH + i];
  }
  results.baseline = runningBaseline / BASELINEFITLENGTH;

  // get the normalization
  if (maxdex - BUFFERZONE + TEMPLATELENGTH > static_cast<int>(trace.size())) {
    results.bad = true;
    return results;
  }
  double runningIntegral = 0;
  for (int i = 0; i < TEMPLATELENGTH; ++i) {
    runningIntegral =
        runningIntegral + trace[maxdex - BUFFERZONE + i] - results.baseline;
  }
  results.integral = runningIntegral;

  results.normalizedAmpl =
      (trace[maxdex] - results.baseline) / results.integral;

  return results;
}

vector<double> correctTrace(const vector<short>& trace, traceSummary summary) {
  vector<double> correctedTrace(trace.size());
  if (summary.bad) {
    for (int i = 0; i < TEMPLATELENGTH; ++i) correctedTrace[i] = 0;
    return correctedTrace;
  }
  for (int i = 0; i < TEMPLATELENGTH; ++i) {
    correctedTrace[i] =
        (trace[summary.peakIndex - BUFFERZONE + i] - summary.baseline) /
        (summary.integral);
  }
  return correctedTrace;
}