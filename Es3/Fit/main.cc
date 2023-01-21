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



using namespace std;

vector<double>* xVecPtr = new vector<double>();

double expPdf(double* xPtr, double par[]){        
    double x = *xPtr;
    double xi = par[0];     // mean of x
    double f = 0;
    if ( x >= 0 && xi > 0. ) { 
    f = (1.0/xi) * exp(-x/xi); 
    }
    return f;
}

double two_expPdf(double* xPtr, double par[]){
    double x = *xPtr;
    double x1 = par[0]; 
    double x2 = par[1]; 
    double alpha = par[2];   
    double f = 0;
    if ( x >= 0 && x1 > 0. && x2 > 0.) { 
        f = 100. * (alpha)*(1.0/x1) * exp(-x/x1) + 100. * (1-alpha) * (1.0/x2) * exp(-x/x2); 
    }
    return f;
}

double int_pdf(double xmin, double xmax, double par[]){
    double x1 = par[0];
    double x2 = par[1];
    double alpha = par[2];

    return alpha * (exp(-xmin/x1) - exp(-xmax/x1)) + (1 - alpha) * (exp(-xmin/x2) - exp(-xmax/x2));

    // return - alpha * exp(-xmax/x1) - (1 - alpha) * exp(-xmax/x2) + alpha * exp(-xmin/x1) + (1 - alpha) * exp(-xmin/x2);
    
}

void getData(vector<double>* xVecPtr){


    ifstream f;
    f.open("../expData_2.txt");
    if ( f.fail() ){
    cout << "Sorry, couldn't open file" << endl;
    exit(1);
    }

    double x ;
    bool acceptInput = true;
    while ( acceptInput ) {
    f >> x;
    acceptInput = !f.eof();
    if ( acceptInput ) {
        xVecPtr->push_back(x);
    }
    }
    f.close();

}


void fcn(int& npar, double* deriv, double& f, double par[], int flag){

    vector<double> xVec = *xVecPtr;          
    int n = xVec.size();

    double lnL = 0.0;
    for (int i=0; i<n; i++){
    double x = xVec[i];
    double pdf = expPdf(&x, par);
    if ( pdf > 0.0 ) {
        lnL += log(pdf);   
    }
    else {
        cout << "WARNING -- pdf is negative!!!" << endl;
    }
    }
    f = -2.0 * lnL;         
 
}                        


void my_fcn(int& npar, double* deriv, double& f, double par[], int flag){

    vector<double> xVec = *xVecPtr;          
    int n = xVec.size();

    double lnL = 0.0;
    for (int i=0; i<n; i++){
    double x = xVec[i];
    double pdf = two_expPdf(&x, par);
    if ( pdf > 0.0 ) {
        lnL += log(pdf);   
    }
    else {
        cout << "WARNING -- pdf is negative!!!" << endl;
    }
    }
    f = -2.0 * lnL;         
 
}        
//-------------------------------------------------------------------------

int main(int argc, char **argv) {

    TApplication app("App", &argc, argv);

    TCanvas canvas;
    canvas.cd();

    gStyle->SetOptFit();
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleSize(0.04);
    gStyle->SetTitleFont(42, "hxy");      
    gStyle->SetLabelFont(42, "xyz");      


    getData(xVecPtr);   

    const int npar = 3;              
    TMinuit minuit(npar);
    minuit.SetFCN(my_fcn);

    double par[npar];               
    double stepSize[npar];           
    double minVal[npar];           
    double maxVal[npar];        

    string parName[npar];

    par[0] = 2.0;
    par[1] = 4.0;
    par[2] = 0.2;

    for (int i = 0; i < npar; i++){
        stepSize[i] = 0.1;
        minVal[i] = 0.0000001;   
        maxVal[i] = 10000000;
    }
    // maxVal[2] = 0.5;
    parName[0] = "x1";
    parName[1] = "x2";
    parName[2] = "alpha";

    for (int i=0; i<npar; i++){
        minuit.DefineParameter(i, parName[i].c_str(), par[i], stepSize[i], minVal[i], maxVal[i]);
    }


    minuit.Migrad();      
    double outpar[npar], err[npar];
    for (int i=0; i<npar; i++){
    minuit.GetParameter(i,outpar[i],err[i]);
    }
    // double covmat [npar] [npar];
    // minuit.mnemat (& covmat [0] [0], npar);

    double covmat [npar][npar];
    minuit.mnemat (& covmat [0][0], npar);
    for (int i = 0; i < 3; i++){
    for( int j = 0; j < 3; j++){
    cout << covmat[i][j] << "\t" ;
    }
    cout << endl;
    }

    cout << "csi_1 = " << outpar[0] << " +- " << err[0] << endl;
    cout << "csi_2 = " << outpar[1] << " +- " << err[1] << endl;
    cout << "alpha = " << outpar[2] << " +- " << err[2] << endl;

    // Plot the result.  For this example plot x values as tick marks.

    double xmin = 0.0;
    double xmax = 27.0;
    TF1* func = new TF1("pdf", two_expPdf, xmin, xmax, npar);
    func->SetParameters(outpar);
    func->Draw();

    func->SetLineStyle(1);             //  1 = solid, 2 = dashed, 3 = dotted
    func->SetLineColor(1);             //  black (default)
    func->SetLineWidth(1);

    func->GetXaxis()->SetTitle("x");
    func->GetYaxis()->SetTitle("f(x;#xi)");

    vector<double> xVec = *xVecPtr;
    const double tickHeight = 0.1;


    int NumBins = 19;
    double vec_bins [NumBins] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 7.0, 8.5, 10.0, 11.5, 13.0, 15.0, 27.};
    TH1D histo ("histo", "histo", NumBins, vec_bins);

    TLine* tick = new TLine();
    for (int i=0; i<xVec.size(); i++){
        tick->DrawLine(xVec[i], 0, xVec[i], tickHeight * 100.);
        tick->SetLineColor(2);
        histo.Fill(xVec[i]);
    }

    canvas.SaveAs("Fit.pdf");
    

    
    // fit.SetParameters(outpar[0], outpar[1], outpar[2]);

    // TFitResultPtr myFitPar = histo.Fit(&fit,"RS");

    // double chi2 = myFitPar->Chi2();
    // cout << chi2 << endl;



    TCanvas c2;
    c2.cd();
    // TF1 fit ("fit", "([2]) * 1./[0] * exp(- x/[0]) + (1-[2]) * 1./[1] * exp(- x/[1])", 0, 24);
    histo.SetTitle("Histogram");
    // histo.Fit(&fit, "R+");
    histo.Draw();
    c2.SaveAs("Histo.pdf");

    double chi2 = 0;
    double c = 1e-9;

    for (int i = 0; i < NumBins; i++){
        double bin_width = vec_bins[i+1] - vec_bins[i];        
        double p = int_pdf(c, c + bin_width, outpar) * 200.;
        // cout << p << endl;
        c += bin_width;
        double observed = histo.GetBinContent(i+1);
        // cout << observed << endl;
        chi2 += pow((observed - p),2)/p;
        
    }

    cout <<  "chi2 = " << chi2 << endl;
    cout << "dof = " << NumBins - 4 << endl;
    cout << "P-Value =  " << TMath::Prob(chi2, NumBins - 4) << endl;
    

    app.Run(true);
    canvas.Close();

    delete tick;
    delete xVecPtr;

    return 0;

}
