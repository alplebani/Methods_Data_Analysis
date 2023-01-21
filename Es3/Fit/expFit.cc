// A simple C++ program to illustrate the use of ROOT class TMinuit 
// for function minimization.  The example shows a Maximum Likelihood
// fit for the mean of an exponential pdf in which TMinuit 
// minimizes - 2*log(L).   The user must specify what to minimize in the 
// function fcn, shown in this file below.

// fcn passes back f = -2*ln L by reference; this is the function to minimize.
// The factor of -2 allows MINUIT to get the errors using the same
// recipe as for least squares, i.e., go up from the minimum by 1.

// TMinuit does not allow fcn to be a member function, and the function
// arguments are fixed, so the one of the only ways to bring the data  
// into fcn is to declare a pointer to the data (xVecPtr) as global.

// For more info on TMinuit see root.cern.ch/root/html/TMinuit.html .

// Glen Cowan
// RHUL Physics
// 4 December 2006

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

using namespace std;

// Declare pointer to data as global (not elegant but TMinuit needs this).

vector<double>* xVecPtr = new vector<double>();

// The pdf to be fitted, here an exponential. 
// First argument needs to be a pointer in order to plot with the TF1 class.

double expPdf(double* xPtr, double par[]){        
  double x = *xPtr;
  double xi = par[0];     // mean of x
  double f = 0;
  if ( x >= 0 && xi > 0. ) { 
    f = (1.0/xi) * exp(-x/xi); 
  }
  return f;
}

//-------------------------------------------------------------------------

//  function to read in the data from a file

void getData(vector<double>* xVecPtr){

  // string infile;
  // cout << "Enter name of input data file: ";
  // cin >> infile;

  ifstream f;
  f.open("../expData2.txt");
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

//-------------------------------------------------------------------------

// fcn passes back f = - 2*ln(L), the function to be minimized.

void fcn(int& npar, double* deriv, double& f, double par[], int flag){

  vector<double> xVec = *xVecPtr;           // xVecPtr is global
  int n = xVec.size();

  double lnL = 0.0;
  for (int i=0; i<n; i++){
    double x = xVec[i];
    double pdf = expPdf(&x, par);
    if ( pdf > 0.0 ) {
      lnL += log(pdf);    // need positive f
    }
    else {
      cout << "WARNING -- pdf is negative!!!" << endl;
    }
  }
  f = -2.0 * lnL;         // factor of -2 so minuit gets the errors right
 
}                         // end of fcn

//-------------------------------------------------------------------------

int main(int argc, char **argv) {

  TApplication theApp("App", &argc, argv);
  TCanvas* canvas = new TCanvas();

// Set a bunch of parameters to make the plot look nice

  // canvas->SetFillColor(0);
  // canvas->UseCurrentStyle();
  // canvas->SetBorderMode(0);       // still leaves red frame bottom and right
  // canvas->SetFrameBorderMode(0);   // need this to turn off red hist frame!
  // gROOT->SetStyle("Plain");
  // canvas->UseCurrentStyle();
  // gROOT->ForceStyle();

  gStyle->SetOptStat(1);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleSize(0.04);
  gStyle->SetTitleFont(42, "hxy");      // for histogram and axis titles
  gStyle->SetLabelFont(42, "xyz");      // for axis labels (values)
  // gROOT->ForceStyle();


  getData(xVecPtr);   

  const int npar = 1;              // the number of parameters
  TMinuit minuit(npar);
  minuit.SetFCN(fcn);

  double par[npar];               // the start values
  double stepSize[npar];          // step sizes 
  double minVal[npar];            // minimum bound on parameter 
  double maxVal[npar];            // maximum bound on parameter
  string parName[npar];

  par[0] = 2.0;            // a guess
  stepSize[0] = 0.2;       // take e.g. 0.1 of start value
  minVal[0] = 0.0000001;   // if min and max values = 0, parameter is unbounded.
  maxVal[0] = 100000000;
  parName[0] = "xi";

  for (int i=0; i<npar; i++){
    minuit.DefineParameter(i, parName[i].c_str(), 
      par[i], stepSize[i], minVal[i], maxVal[i]);
  }

// Do the minimization!

  minuit.Migrad();       // Minuit's best minimization algorithm
  double outpar[npar], err[npar];
  for (int i=0; i<npar; i++){
    minuit.GetParameter(i,outpar[i],err[i]);
  }

  cout << "csi = " << outpar[0] << " +- " << err[0] << endl;

// Plot the result.  For this example plot x values as tick marks.

  double xmin = 0.0;
  double xmax = 5.0;
  TF1* func = new TF1("funcplot", expPdf, xmin, xmax, npar);
  func->SetParameters(outpar);
  func->Draw();

  func->SetLineStyle(1);             //  1 = solid, 2 = dashed, 3 = dotted
  func->SetLineColor(1);             //  black (default)
  func->SetLineWidth(1);

  func->GetXaxis()->SetTitle("x");
  func->GetYaxis()->SetTitle("f(x;#xi)");

  vector<double> xVec = *xVecPtr;
  const double tickHeight = 0.1;
  TLine* tick = new TLine();
  for (int i=0; i<xVec.size(); i++){
    tick->DrawLine(xVec[i], 0, xVec[i], tickHeight);
  }

  theApp.Run(true);
  canvas->Close();

  delete canvas;
  delete tick;
  delete xVecPtr;

  return 0;

}
