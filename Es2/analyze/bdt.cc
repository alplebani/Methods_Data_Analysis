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
#include "TApplication.h"
#include "TCanvas.h"
#include "TLegend.h"
// #include "TColors.h"

using namespace std;

OutFunc* testStat;      

int main() {

    TApplication app ("app", 0, 0);

    TFile* histFile = new TFile("analysis.root", "RECREATE");  
    TH1D* hSig = new TH1D ("hSig", "BDT, signal", 100, -5.0, 5.0);
    TH1D* hBkg = new TH1D ("hBkg", "BDT, background", 100, -5.0, 5.0);
    TH1D h_Sig ("h_Sig", "BDT signal", 100, -1., 1.0);
    TH1D h_Bkg ("h_Bkg", "BDT background", 100, -1., 1.0);
    TH1D* hSig_test = new TH1D ("hSig_test", "BDT, signal test", 100, -5.0, 5.0);
    TH1D* hBkg_test = new TH1D ("hBkg_test", "BDT, background test", 100, -5.0, 5.0);
    TH1D h_Sig_test ("h_Sig_test", "BDT signal test", 100, -1., 1.0);
    TH1D h_Bkg_test ("h_Bkg_test", "BDT background test", 100, -1., 1.0);
    TList* hList = new TList();      
    hList->Add(hSig);
    hList->Add(hBkg);

    std::string dir    = "../train/dataset/weights/";
    std::string prefix = "tmvaTest";
    const double tCut = 0.;
    std::vector<bool> useVar(3);
    useVar[0] = true;     
    useVar[1] = true;     
    useVar[2] = true;     
    testStat = new OutFunc("BDT200", dir, prefix, tCut, useVar);

    TFile* inputFile = new TFile("../generate/testData.root");
    // TFile* inputFile = new TFile("../generate/trainingData.root");
    inputFile->ls();
    TTree* sig = dynamic_cast<TTree*>(inputFile->Get("sig"));
    TTree* bkg = dynamic_cast<TTree*>(inputFile->Get("bkg"));
    std::vector<TTree*> treeVec;
    treeVec.push_back(sig);
    treeVec.push_back(bkg);

    int nSig, nBkg;
    int nSigAcc = 0;
    int count_s = 0;
    int count_b = 0;
    int numEntries = 0; 

    for (int i=0; i<treeVec.size(); i++){

        treeVec[i]->Print();
        Event evt;
        treeVec[i]->SetBranchAddress("evt", &evt);
        numEntries = treeVec[i]->GetEntries();
        if ( i == 0 ) { nSig = numEntries; }
        if ( i == 1 ) { nBkg = numEntries; }
        std::cout << "number of entries = " << numEntries << std::endl;

        for (int j=0; j<numEntries; j++){

            treeVec[i]->GetEntry(j);                   
            double t = testStat->val(&evt);            
            
            if ( i == 0 ){                       
                h_Sig.Fill(t);
                if(t > 0.5){
                    count_s++;
                }
                if ( t >= tCut ) { nSigAcc++; }
            }
            else if ( i == 1 ){ 
                if (t > 0.5){
                    count_b++;
                }                 
                h_Bkg.Fill(t);
            }


            // Add your code here to cound number of times that t is greater
            // or less than a given threshold for each hypothesis; from these
            // compute below the fraction of events with t >= tCut for each class.




            

        }

    }

    double epsSig = static_cast<double>(nSigAcc)/static_cast<double>(nSig);
    cout << "nSigAcc, nSig = " << nSigAcc << " , " << nSig << endl;
    std::cout << "signal efficiency (power) = " << epsSig << std::endl;

    gStyle->SetOptStat(0);

    TCanvas sig_bkg = ("S_B", "S_B");
    sig_bkg.cd();
    h_Sig.SetLineColor(kBlue);
    h_Bkg.SetLineColor(kRed);
    h_Sig.SetXTitle("O_{BDT}");
    // h_Sig.GetXaxis()->SetRangeUser(0.5,1);
    // h_Bkg.GetXaxis()->SetRangeUser(0.5,1);
    auto legend = new TLegend(0.8,0.75,0.95,0.85);
    legend->AddEntry(&h_Sig,"Signal","l");
    legend->AddEntry(&h_Bkg,"Background","l");
    h_Sig.SetTitle("BDT discriminant, test dataset");
    h_Sig.Draw();
    h_Bkg.Draw("SAME");
    legend->Draw("SAME");

    // TCanvas cut = ("cut", "cut");
    // cut.cd();
    // h_Sig.SetLineColor(kBlue);
    // h_Bkg.SetLineColor(kRed);
    // h_Sig.GetXaxis()->SetRangeUser(0.5,1);
    // h_Bkg.GetXaxis()->SetRangeUser(0.5,1);
    // h_Sig.SetXTitle("O_{MLP}");
    // auto legend_2 = new TLegend(0.7,0.75,0.85,0.85);
    // legend_2->AddEntry(&h_Sig,"Signal","l");
    // legend_2->AddEntry(&h_Bkg,"Background","l");
    // h_Sig.SetTitle("MLP discriminant");
    // h_Sig.Draw();
    // h_Bkg.Draw("SAME");
    // legend_2->Draw("SAME");


    sig_bkg.SaveAs("BDT_test.pdf");

    cout << "Efficiency of the signal P(t>0.5|s) = " << 1.*count_s/h_Sig.GetEntries() << endl;
    cout << "Efficiency of the background P(t>0.5|b) = " << 1.*count_b/h_Bkg.GetEntries() << endl;
    cout << "Signal Purity P(s|t>0.5) = " << 1.*(count_s)/(count_s+count_b) << endl;

    inputFile->Close();
    histFile->Write();
    histFile->Close();

    
    



    app.Run();

    return 0;

}
