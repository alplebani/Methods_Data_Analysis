//  tmvaTrain.cc P. Manek and G. Cowan, 2019
//  simple program to train TMVA classifier (here, Fisher)

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>

#include <TFile.h>
#include <TString.h>

#include <TMVA/DataLoader.h>
#include <TMVA/Factory.h>

using namespace std;

int main(){

// Create ouput file, factory object and open the input file

  TFile* outputFile = TFile::Open("TMVA.root", "RECREATE");
  TMVA::Factory* factory = new TMVA::Factory("tmvaTest", outputFile, "");
  TMVA::DataLoader* dataLoader = new TMVA::DataLoader("dataset");
  TFile* trainingFile = new TFile("../generate/trainingData.root");
  TFile* testFile = new TFile("../generate/testData.root");

// get the TTree objects from the input files

  TTree* sigTrain = (TTree*)trainingFile->Get("sig");
  TTree* bkgTrain = (TTree*)trainingFile->Get("bkg");
  int nSigTrain = sigTrain->GetEntries();
  int nBkgTrain = bkgTrain->GetEntries();

  TTree* sigTest = (TTree*)testFile->Get("sig");
  TTree* bkgTest = (TTree*)testFile->Get("bkg");
  int nSigTest = sigTest->GetEntries();
  int nBkgTest = bkgTest->GetEntries();

// global event weights (see below for setting event-wise weights)

  double sigWeight = 1.0;
  double bkgWeight = 1.0;
  

// Define the input variables that shall be used for the MVA training
// (the variables used in the expression must exist in the original TTree).

  

// Book MVA methods (see TMVA manual).

   //factory->BookMethod(dataLoader, TMVA::Types::kFisher,"Fisher", "H:!V:Fisher");
  //factory->BookMethod(dataLoader, TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5");
  int n_trees [12] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 10000, 50000};
  vector<string> names;
  for (int i = 0; i < 12; i++){
    names.push_back("BDT" + std::__cxx11::to_string(n_trees[i]));
  }

  for (int i = 0; i < 12; i++){
    TMVA::Factory* factory = new TMVA::Factory("tmvaTest", outputFile, "");
    TMVA::DataLoader* dataLoader = new TMVA::DataLoader("dataset");
    dataLoader->AddSignalTree(sigTrain, sigWeight, TMVA::Types::kTraining);
    dataLoader->AddBackgroundTree(bkgTrain, bkgWeight, TMVA::Types::kTraining);
    dataLoader->AddSignalTree(sigTest, sigWeight, TMVA::Types::kTesting);
    dataLoader->AddBackgroundTree(bkgTest, bkgWeight, TMVA::Types::kTesting);
    dataLoader->AddVariable("x", 'F');
    dataLoader->AddVariable("y", 'F');
    dataLoader->AddVariable("z", 'F');
    factory->BookMethod(dataLoader, TMVA::Types::kBDT, names[i], "NTrees=" + std::__cxx11::to_string(n_trees[i]) + ":BoostType=AdaBoost");
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

  // Save the output and finish up

    outputFile->Close();
    std::cout << "==> wrote root file TMVA.root" << std::endl;
    std::cout << "==> TMVAnalysis is done!" << std::endl;

    delete factory;
    delete dataLoader;

  }

  
// Train, test and evaluate all methods

  return 0;

}
