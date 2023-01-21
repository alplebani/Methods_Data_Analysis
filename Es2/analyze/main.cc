#include <iostream>
#include <fstream>
#include <cstdlib>
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
#include "TGraph.h"
// #include "TColors.h"

using namespace std;

OutFunc* testStat;      

int main() {

    TApplication app ("app", 0, 0);

    int n_trees [12] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 10000, 50000};
    vector<string> names;
    for (int i = 0; i < 12; i++){
        names.push_back("BDT" + std::__cxx11::to_string(n_trees[i]));
    }

    double error_s [12];
    double error_b [12];

    for (int valore = 0; valore < 12; valore++){
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
        testStat = new OutFunc(names[valore], dir, prefix, tCut, useVar);

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
                    if(t < 0){
                        count_s++;
                    }
                    if ( t >= tCut ) { nSigAcc++; }
                }
                else if ( i == 1 ){ 
                    if (t > 0){
                        count_b++;
                    }                 
                    h_Bkg.Fill(t);
                }                

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
        string title = "BDT discriminant, test dataset, " + std::__cxx11::to_string(n_trees[valore]);
        h_Sig.SetTitle(title.c_str());
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

        string pdf_title = "BDT_" + std::__cxx11::to_string(n_trees[valore]) + "test.pdf";
        sig_bkg.SaveAs(pdf_title.c_str());

        error_s [valore] = 1.*(count_s)/(h_Sig.GetEntries());
        error_b [valore] = 1.*(count_b)/h_Bkg.GetEntries();

        inputFile->Close();
        histFile->Write();
        histFile->Close();
    }

    TGraph gr_errors_s;
    TCanvas c_errors_s ("c_err_s", "c_err_s");
    c_errors_s.cd();

    for (int i = 0; i < 12; i++){
        gr_errors_s.SetPoint(i, n_trees[i], error_s[i]);
    }

    gr_errors_s.GetXaxis()->SetTitle("Number of trees");
    gr_errors_s.GetYaxis()->SetTitle("Error");
    gPad->SetLogx();
    gr_errors_s.SetTitle("Errors vs number of trees, test dataset, signal");
    gr_errors_s.Draw("ALP*");

    c_errors_s.SaveAs("errors_test_signal.pdf");

    TGraph gr_errors_b;
    TCanvas c_errors_b ("c_err_b", "c_err_b");
    c_errors_b.cd();

    for (int i = 0; i < 12; i++){
        gr_errors_b.SetPoint(i, n_trees[i], error_b[i]);
    }

    gr_errors_b.GetXaxis()->SetTitle("Number of trees");
    gr_errors_b.GetYaxis()->SetTitle("Error");
    gPad->SetLogx();
    gr_errors_b.SetTitle("Errors vs number of trees, test dataset, background");
    gr_errors_b.Draw("ALP*");

    c_errors_b.SaveAs("errors_test_bkg.pdf");

    app.Run();

    return 0;

}
