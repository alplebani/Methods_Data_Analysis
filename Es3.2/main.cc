#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>

#include <TMinuit.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TF1.h>
#include <TAxis.h>
#include <TLine.h>
#include "TH1D.h"
#include "TFitResult.h"
#include "Riostream.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TLatex.h"
#include "TPostScript.h"

using namespace std;

// Simple polynomial for test fit; modify as needed.


// int npars = [2, 3, 4];

const int npar = 4;


double fitFunc (double xArray[], double par[]){
  double x = xArray[0];
  double f = 0.;
  for (int i=0; i<npar; i++){
    f += par[i] * pow(x,i);
  }
  return f;
}

double var (double x, TMatrixTSym<double> V){
    double variance = 0.;
    double my_x [npar];
    for (int i = 0; i < npar; i++){
        my_x[i] = pow(x, i);
    }
    for (int i = 0; i < npar; i++){
        for (int j = 0; j < npar; j++){
            variance += my_x [i] * V[i][j] * my_x [j];
        }
    }
    return variance;
}

int main(int argc,char** argv) {

    // int npar = 3;

    // const int npar = atoi(argv[1]);

    TApplication app("app", 0, 0);

    // first make a canvas and a 2D histogram for the axes

    TCanvas* canvas = new TCanvas("canvas", "canvas");

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    double xMin = 0.;
    double xMax = 10.;
    double yMin = 0.;
    double yMax = 6.;
    TH2F* axhist = new TH2F("axhist", "title", 10, xMin, xMax, 10, yMin, yMax);
    axhist->SetTitle("Fit");
    axhist->SetXTitle("x");
    axhist->SetYTitle("y");

    const int maxCol = 10;
    const int maxRow = 500;

    double u[maxCol];
    double dataArray[maxCol][maxRow];


    ifstream inFile;
    inFile.open("testData.txt");
    if (inFile.fail()) { 
        cout << "Couldn't open file!" << endl;
        exit(1); 
    }

    bool readLine = true;
    int lineNum = 0;
    int ncol;
    while ( readLine ){

        TString line;
        stringstream ss;
        line.ReadLine(inFile);
        readLine = inFile.good();

        if ( readLine ) {

        TString firstChar = line(0,1);
        bool useLine = firstChar != "#";       // # for comment line

        if ( useLine ){

            int i = 0;
            stringstream ss;
            ss << line;              // put whole line into ss
            TString token;
            bool getToken = true;

            while ( getToken ) {
            ss >> token;           // extracts one token
            if ( token.Length() > 0 ) {
                u[i] = token.Atof();
                i++;
            } 
            else {
                getToken = false;
            }
            }             // getToken
            ncol = i;  

            for (int i=0; i<ncol; i++){
            dataArray[i][lineNum] = u[i];
            }
            lineNum++;

        }               // useLine

        }         // readLine

    }           // readLine

    int numPoints = lineNum;
    inFile.close();

    double* x = dataArray[0];
    double* y = dataArray[1];
    double* dx(0);       // null pointer means errors set to zero
    double* dy = dataArray[2];
    TGraphErrors* tge = new TGraphErrors(numPoints, x, y, dx, dy);

    double xmin = 0.;
    double xmax = 10.;
    TF1* f = new TF1("f", fitFunc, xmin, xmax, npar);
    TFitResultPtr r = tge->Fit(f, "S0");    // needed for covariances



    double chi2 = f->GetChisquare();
    int ndof = f->GetNDF();
    double pval = TMath::Prob(chi2, ndof);
    cout << "chi2 = " << chi2 << " for " << ndof 
            << " degrees of freedom" << endl;
    cout << "p-value = " << pval << endl;
    cout << "par number, value, error..." << endl;
    double thetaHat[npar];
    double sigmaThetaHat[npar];
    for (int i=0; i<npar; i++){
        thetaHat[i] = f->GetParameter(i);
        sigmaThetaHat[i] = f->GetParError(i);
        cout << i << "  " << thetaHat[i] << " +- " << sigmaThetaHat[i] << endl;
    }
    cout << endl;

    cout << "Covariance matrix and its inverse..." << endl;
    TMatrixDSym V = r->GetCovarianceMatrix();
    TMatrixDSym Vinv(V);           // initialize Vinv with V
    Vinv.Invert();                 // writes inverse into Vinv
    for (int i=0; i<npar; i++){
        for (int j=0; j<npar; j++){
        cout << i << "  " << j << "  " << V[i][j] << "  " << Vinv[i][j] << endl;
        }
    }
    cout << endl;


    tge->SetLineColor(1);
    tge->SetLineWidth(2);
    tge->SetLineStyle(1);
    tge->SetMarkerColor(1);
    tge->SetMarkerSize(0.8);
    tge->SetMarkerStyle(20);
    tge->SetTitle("Fit using 4 parameters");
    tge->Draw("AP");              // Z turns off error bar tick marks

    f->SetRange(xmin, xmax);
    f->SetLineColor(2);
    f->SetLineWidth(2);
    f->SetLineStyle(1);
    f->GetXaxis()->SetTitle("x");
    f->GetYaxis()->SetTitle("f(x,#theta");
    f->Draw("same");

    TLegend* leg = new TLegend(0.2, 0.8, 0.35, 0.9); 
    leg->AddEntry(tge, "test data", "p");  // l for line, f for box, p for point
    leg->AddEntry(f, "fit result", "l");
    leg->Draw();

    // TLatex* tl = new TLatex();
    // tl->SetTextAlign(11);
    // tl->SetTextSize(0.04);
    // tl->SetTextFont(42);
    // tl->SetNDC();
    // tl->DrawLatex(.65, 0.78, "A simple fit");  

    TPostScript psfile("plot.eps", 113);     
    canvas->SetTitle("Fit");
    canvas->Draw();
    string title = "fit_result_" + std::__cxx11::to_string(npar) + ".pdf";
    // canvas -> SaveAs(title.c_str());
    psfile.Close();
    // canvas->Print("plot.gif", "gif");


    double p_values [4] = {1.36e-16, 0.001427, 0.13764, 0.167811};
    int num_pars [4] = {2, 3, 4, 5};

    TCanvas c ("c", "c");
    c.cd();
    TGraph graph;

    for (int i = 0; i < 4; i++){
        graph.SetPoint(i, num_pars[i], p_values[i]);
    }
    graph.GetXaxis()->SetTitle("Number of parameters");
    graph.GetYaxis()->SetTitle("p-value");
    graph.GetYaxis()->SetRangeUser(1e-18,1);
    graph.GetXaxis()->SetRangeUser(1.8,5.2);
    TLine line (1.8, 0.1, 5.2, 0.1);
    graph.SetTitle("p-values");
    gPad->SetLogy();
    graph.SetLineColor(2);
    graph.Draw("ALP*");
    line.SetLineColor(4);
    line.Draw("SAME");

    // c.SaveAs("p_values.pdf");

    for (int i = 0; i < npar; i++){
        f -> SetParameter (i , thetaHat[i]);
    }

    double values [3] = {5, 6, 10};

    cout << "Using " << npar << " parameters" << endl;

    for (int i = 0; i < 3; i++){
        cout << "f(" << values[i] << ")=" << f->Eval(values[i]) << endl;
    } 

    double a = 6.0;
    double b = 6.0;
    double a_2 = 10.;
    double b_2 = 5.;

    double d_1 = f->Eval(a) - f->Eval(b);
    double d_2 = f->Eval(a_2) - f->Eval(b_2);

    double sigma_1 = 0;
    sigma_1 = sqrt(var(f->Eval(a), V) + var(f->Eval(b), V));

    double sigma_2 = 0.;
    sigma_2 = sqrt(var(f->Eval(a_2), V) + var(f->Eval(b_2), V));

    cout << "Using " << npar << " parameters" << endl;
    cout << "d = " << d_1 << " +- " << sigma_1 << endl;
    cout << "d = " << d_2 << " +- " << sigma_2 << endl;

    double mod_theta [4] = {-0.75, 2.5, -0.5, 0.026};

    double new_chi2 = 0.;
    double temp_i = 0.;
    double temp_j = 0.;

    for (int i = 0; i < npar; i++){
        for (int j = 0; j < npar; j++){
            temp_i = mod_theta [i] - thetaHat [i];
            temp_j = mod_theta [j] - thetaHat [j];
            new_chi2 += temp_i * Vinv[i][j] * temp_j; 
        }
    }  

    cout << "chi2 = " << new_chi2 << endl;
    cout << "dof = 3 " << endl;
    cout << "p-value = " << TMath::Prob(new_chi2, 3) << endl;

    gStyle->SetOptFit(0);

    TCanvas* canvas_f = new TCanvas("canvas_f", "canvas_f");
    canvas_f->cd();

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    TH2F* axhist_f = new TH2F("axhist_f", "Model", 10, xMin, 12, 10, yMin, yMax);
    axhist_f->SetXTitle("x");
    axhist_f->SetTitle("Model");
    axhist_f->SetYTitle("y");
    TF1* f_2 = new TF1("f", fitFunc, xmin, 12, npar);
    
    for (int i = 0; i < npar; i++){
        f_2 -> SetParameter(i, mod_theta[i]);
    }

    
    tge->SetLineColor(1);
    tge->SetLineWidth(2);
    tge->SetLineStyle(1);
    tge->GetYaxis()->SetRangeUser(0, 4);
    tge->GetXaxis()->SetRangeUser(0,12);
    tge->SetMarkerColor(1);
    tge->SetMarkerSize(0.8);
    tge->SetMarkerStyle(20);
    tge->SetTitle("Fit using the model");
    tge->Draw("AP");  

    f_2->SetRange(xmin, xmax);
    f_2->GetYaxis()->SetRangeUser(0, 4);
    f_2->SetLineColor(2);
    f_2->SetLineWidth(2);
    f_2->SetLineStyle(1);
    f_2->GetXaxis()->SetTitle("x");
    f_2->GetYaxis()->SetTitle("f(x,#theta");
    f_2->Draw("SAME");
    TLegend* leg_2 = new TLegend(0.2, 0.8, 0.35, 0.9); 
    leg_2->AddEntry(tge, "Data", "p");  // l for line, f for box, p for point
    leg_2->AddEntry(f_2, "Model", "l");
    leg_2->Draw();

    canvas_f->SaveAs("final_fit.pdf");


    app.Run();

    return 0;

}


