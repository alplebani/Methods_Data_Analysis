#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TPad.h>
#include <TAxis.h>
#include <TH2F.h>
#include <TF2.h>
#include <TLine.h>
#include "Event.h"
#include "OutFunc.h"
#include <TMVA/Reader.h>

using namespace std;

OutFunc* testStat;      // need global for contour later

int main() {

// Set up an output file and book some histograms

  TFile* histFile = new TFile("analysis.root", "RECREATE");  
  TH1D* hSig = new TH1D("hSig", "Fisher, signal", 100, -5.0, 5.0);
  TH1D* hBkg = new TH1D("hBkg", "Fisher, background", 100, -5.0, 5.0);
  TList* hList = new TList();      // list of histograms to store
  hList->Add(hSig);
  hList->Add(hBkg);

// Set up the OutFunc object.  First argument must be one of the classifiers.
// 4th argument is offset for contour.
// 5th argument is bool array indicating which variables were used in training

  std::string dir    = "../train/dataset/weights/";
  std::string prefix = "tmvaTest";
  const double tCut = 0.;
  std::vector<bool> useVar(3);
  useVar[0] = true;      // x
  useVar[1] = true;      // y
  useVar[2] = true;      // z
  testStat = new OutFunc("Fisher", dir, prefix, tCut, useVar);

// Open input file, get the TTrees, put into a vector

  TFile* inputFile = new TFile("../generate/testData.root");
  inputFile->ls();
  TTree* sig = dynamic_cast<TTree*>(inputFile->Get("sig"));
  TTree* bkg = dynamic_cast<TTree*>(inputFile->Get("bkg"));
  std::vector<TTree*> treeVec;
  treeVec.push_back(sig);
  treeVec.push_back(bkg);

// Loop over TTrees, i=0 is signal, i=1 is background

  int nSig, nBkg;
  int nSigAcc = 0;
  for (int i=0; i<treeVec.size(); i++){

    treeVec[i]->Print();
    Event evt;
    treeVec[i]->SetBranchAddress("evt", &evt);
    int numEntries = treeVec[i]->GetEntries();
    if ( i == 0 ) { nSig = numEntries; }
    if ( i == 1 ) { nBkg = numEntries; }
    std::cout << "number of entries = " << numEntries << std::endl;

// Loop over events.

    for (int j=0; j<numEntries; j++){

      treeVec[i]->GetEntry(j);                   // sets evt
      double t = testStat->val(&evt);            // evaluate test statistic
      // cout << t << " " << evt.x << "  " << evt.y << endl;
      
      if ( i == 0 ){                       // signal
        hSig->Fill(t);
	if ( t >= tCut ) { nSigAcc++; }
      }
      else if ( i == 1 ){                  // background
        hBkg->Fill(t);
      }


      // Add your code here to cound number of times that t is greater
      // or less than a given threshold for each hypothesis; from these
      // compute below the fraction of events with t >= tCut for each class.




      

    }

  }

//  Compute efficiencies (power, size) 

  double epsSig = static_cast<double>(nSigAcc)/static_cast<double>(nSig);
  cout << "nSigAcc, nSig = " << nSigAcc << " , " << nSig << endl;
  std::cout << "signal efficiency (power) = " << epsSig << std::endl;

  
// Close up

  inputFile->Close();
  histFile->Write();
  histFile->Close();

  return 0;

}
