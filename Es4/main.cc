#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>

#include <TMinuit.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TF1.h>
#include <TH1.h>
#include <TAxis.h>
#include <TLine.h>
#include <TRandom3.h>
#include <Math/ProbFuncMathCore.h>
#include <Math/PdfFuncMathCore.h>
#include <Math/QuantFuncMathCore.h>
#include <Math/Random.h>

using namespace std;


vector<double>* xVecPtr = new vector<double>();


double pdf(double* xPtr, double par[]){
  double x = *xPtr;
  double theta = par[0];
  double xi = par[1];
  double mu = par[2];
  double sigma = par[3];
  double xMax = par[4];
  double normConst = ROOT::Math::gaussian_cdf(xMax, sigma, mu) -
    ROOT::Math::gaussian_cdf(0., sigma, mu);
  double f = 0;
  if (x >= 0 && x <= xMax && xi > 0) {
    double lambda = 1/xi;
    f = theta * ROOT::Math::gaussian_pdf(x, sigma, mu) / normConst  +
      (1. - theta) * ROOT::Math::exponential_pdf(x, lambda) /
      (1. - exp(-lambda*xMax));
  }
  return f;
}

//-------------------------------------------------------------------------

void generateData(vector<double>* xVecPtr, double parGen[], int numVal){

  int seed = 12345;
  TRandom3 ran(seed);

  double theta = parGen[0];
  double xi = parGen[1];
  double mu = parGen[2];
  double sigma = parGen[3];
  double xMax = parGen[4];

  for (int i = 0; i<numVal; ++i){
    double r1 = ran.Rndm();
    double x =  0.;
    if (r1 < theta){
      bool gotValue = false;
      while(!gotValue){
	x = ran.Gaus(mu, sigma);
	gotValue = x >= 0. && x <= xMax;
      }
    }
    else {
      double r2 = ran.Rndm();
      x = - xi * log(1. - r2*(1. - exp(-xMax/xi)));
    }
    xVecPtr->push_back(x);
  }

}


void fcn(int& npar, double* deriv, double& fObj, double par[], int flag){

  vector<double> xVec = *xVecPtr;           
  int n = xVec.size();

  double lnL = 0.0;
  for (int i=0; i<n; i++){
    double x = xVec[i];
    double f = pdf(&x, par);
    if (f > 0.0) {
      lnL += log(f);    
    }
    else {
      cout << "WARNING -- pdf is negative!!!" << endl;
    }
  }
  fObj = - 2.*lnL;         

}                         

//-------------------------------------------------------------------------

int main(int argc, char **argv) {

    TApplication app("App", 0, 0);

    TCanvas canvas ("canvas", "canvas");
    canvas.cd();

    double theta = 0.2;           // signal fraction
    double xi = 5.;               // mean of exponential
    double mu = 10.;              // mean of Gaussian
    double sigma = 2.;            // std. dev. of Gaussian
    double xMax = 20.;

    const int npar = 5;  
    double parGen[npar];
    parGen[0] = theta;
    parGen[1] = xi;
    parGen[2] = mu;
    parGen[3] = sigma;
    parGen[4] = xMax;

    // int numVal = atoi(argv[1]);
    int numVal = 200;
    generateData(xVecPtr, parGen, numVal);
    vector<double> xVec = *xVecPtr;

    TMinuit minuit(npar);
    minuit.SetFCN(fcn);

    double par[npar];               // the start values
    double stepSize[npar];          // step sizes 
    double minVal[npar];            // minimum bound on parameter 
    double maxVal[npar];            // maximum bound on parameter
    string parName[npar];

    

    par[0] = 0.4;             // a guess
    stepSize[0] = 0.04;       // take e.g. 0.1 of start value
    minVal[0] = 0.;           // both min/max values zero means no limit
    maxVal[0] = 1.;
    parName[0] = "theta";

    par[1] = 4.;             
    stepSize[1] = 0.4; 
    minVal[1] = 1.e-3;
    maxVal[1] = 1.e3;
    parName[1] = "xi";

    par[2] = mu;
    stepSize[2] = 0.1;     // fixed
    minVal[2] = 0;
    maxVal[2] = 100000;
    parName[2] = "mu";

    par[3] = sigma;
    stepSize[3] = 0.1;     // fixed
    minVal[3] = 0.;
    maxVal[3] = 100000;
    parName[3] = "sigma";

    par[4] = xMax;
    stepSize[4] = 0.;     // fixed
    minVal[4] = 0.;
    maxVal[4] = 0.;
    parName[4] = "xMax";

    

    for (int i=0; i<npar; i++){
        minuit.DefineParameter(i, parName[i].c_str(), 
        par[i], stepSize[i], minVal[i], maxVal[i]);
    }

    // Do the minimization
    // minuit.FixParameter(1);
    minuit.FixParameter(2);
    minuit.FixParameter(3);
    minuit.Migrad();
    double MLE[npar], sigmaMLE[npar];
    for (int i=0; i<npar; i++){
        minuit.GetParameter(i, MLE[i], sigmaMLE[i]);
    }

    cout << "par. index, MLE and std. dev.:" << endl;
    for (int i=0; i<1; i++){
        cout << i << "  " << MLE[i] << "  " << sigmaMLE[i] << endl;
    }
    cout << endl;

    // int nparFree = minuit.GetNumFreePars();
    // double cov[nparFree][nparFree];
    // double rho[nparFree][nparFree];
    // minuit.mnemat(&cov[0][0], nparFree);
    // cout << "par. indices, cov, correlation coef.:";
    // // for (int i=0; i<nparFree; i++){
    //     for (int j=0; j<nparFree; j++){
    //         rho[i][j] = cov[i][j] / sqrt(cov[i][i]*cov[j][j]);
    //         cout << i << "  " << j << "  " << cov[i][j] << "  " << rho[i][j] << endl;
    //     }
    // }

    // Plot the result.

    // TF1* func = new TF1("", pdf, 0., xMax, npar);
    // func->SetParameters(MLE);

    // TAxis* xa = func->GetHistogram()->GetXaxis();
    // TAxis* ya = func->GetHistogram()->GetYaxis();
    // xa->SetTitleOffset(1.2);    //  factor multiplies default offset
    // ya->SetTitleOffset(1.5);
    // xa->SetLabelOffset(0.005);
    // ya->SetLabelOffset(0.02);

    // func->SetLineStyle(1);             //  1 = solid, 2 = dashed, 3 = dotted
    // func->SetLineColor(2);             //  black (default)
    // func->SetLineWidth(3);
    // func->SetTitle("pdf");
    // func->GetXaxis()->SetTitle("x");
    // func->GetYaxis()->SetTitle("f(x; #theta, #xi)");
    // func->Draw();

    // double zero = 0.;
    // double yMax = pdf(&zero, MLE)*1.1;
    // func->SetMinimum(0.);
    // func->SetMaximum(yMax);
    // const double tickHeight = yMax*0.05;
    // TLine* tick = new TLine();
    // for (int i=0; i<xVec.size(); i++){
    //     if ( xVec[i] <= xMax ){
    //         tick->SetLineColor(4);
    //         tick->DrawLine(xVec[i], 0., xVec[i], tickHeight);
    //     }
    // }
    // // canvas.Update();
    // canvas.Show();

    // double best_theta = 0.20289;
    // double sigma_best_theta = 5.44037e-02;
    // double best_xi = 4.8911;
    // double sigma_best_xi = 6.26684e-01;

    // // Make -2*logL scan for theta
    // int ierr;
    // double arglist[4];
    // arglist[0] = 2;      // par no.  (starts at 1, not 0)
    // arglist[1] = 100;     // num points
    // arglist[2] = MLE[1] - 0.1;    // from
    // arglist[3] = MLE[1] + 0.1;     // to
    // minuit.mnexcm("SCAN", arglist, 4, ierr);
    // TCanvas c1 ("c1", "c1");
    // TGraph* tg1 = (TGraph*)minuit.GetPlot();
    // tg1->GetXaxis()->SetTitle("#theta");
    // // tg1->GetXaxis()->SetRangeUser(0.09, 0.31);
    // tg1->GetYaxis()->SetRangeUser(1060, 1062);
    // tg1->GetYaxis()->SetTitle("-2 ln L(#theta)");
    // tg1->SetTitle("Likelihood scan");
    // tg1->SetLineColor(2);
    // tg1->SetLineWidth(2);
    // TAxis* xa1 = tg1->GetHistogram()->GetXaxis();
    // TAxis* ya1 = tg1->GetHistogram()->GetYaxis();
    // xa1->SetTitleOffset(1.2);    //  factor multiplies default offset
    // ya1->SetTitleOffset(1.2);
    // xa1->SetLabelOffset(0.005);
    // ya1->SetLabelOffset(0.02);
    // TLine l1 (0.09, 1060.441, 0.31, 1060.441);
    // l1.SetLineColor(4);
    // TLine l2 (0.09, 1061.441, 0.31, 1061.441);
    // l2.SetLineColor(4);
    // TLine l3 (best_xi - sigma_best_xi, 1060, best_xi- sigma_best_xi, 1067);
    // TLine l4 (best_xi + sigma_best_xi, 1060, best_xi + sigma_best_xi, 1067);
    // l3.SetLineColor(4);
    // l4.SetLineColor(4);    
    // tg1->Draw();
    // l1.Draw("SAME");
    // l2.Draw("SAME");
    // l3.Draw("SAME");
    // l4.Draw("SAME");
    // c1.Show();

    // // Make contour plot
    // TCanvas c2 ("c2", "c2");
    // // c2->UseCurrentStyle();
    // TGraph* tg2 = (TGraph*)minuit.Contour(100);
    // tg2->GetXaxis()->SetTitle("#theta");
    // tg2->GetYaxis()->SetTitle("#xi");
    // tg2->SetTitle("#xi vs #theta");
    // tg2->SetLineColor(2);
    // tg2->SetLineWidth(2);
    // tg2->GetXaxis()->SetRangeUser(0.13, 0.28);
    // tg2->GetYaxis()->SetRangeUser(4.2, 5.7);
    // TAxis* xa2 = tg2->GetHistogram()->GetXaxis();
    // TAxis* ya2 = tg2->GetHistogram()->GetYaxis();
    // xa2->SetTitleOffset(1.2);    //  factor multiplies default offset
    // ya2->SetTitleOffset(1.2);
    // xa2->SetLabelOffset(0.005);
    // ya2->SetLabelOffset(0.02);
    // tg2->Draw();
    // TLine l_theta (best_theta + sigma_best_theta, 4.2, best_theta + sigma_best_theta, 5.7);
    // TLine l_theta_2 (best_theta - sigma_best_theta, 4.2, best_theta - sigma_best_theta, 5.7);
    // TLine l_xi (0.13, best_xi + sigma_best_xi,0.28, best_xi + sigma_best_xi);
    // TLine l_x2 (0.13, best_xi - sigma_best_xi, 0.28, best_xi - sigma_best_xi);
    // l_theta.SetLineColor(4);
    // l_theta_2.SetLineColor(4);
    // l_xi.SetLineColor(4);
    // l_x2.SetLineColor(4);
    // l_theta.Draw("SAME");
    // l_theta_2.Draw("SAME");
    // l_xi.Draw("SAME");
    // l_x2.Draw("SAME");
    // c2.Show();

    // string title1 = "distr_" + std::__cxx11::to_string(numVal) + ".pdf";
    // string title2 = "scan_" + std::__cxx11::to_string(numVal) + ".pdf";
    // string title3 = "contour_" + std::__cxx11::to_string(numVal) + ".pdf";
    // canvas.SaveAs("other_scan.pdf");
    // c1.SaveAs(title2.c_str());
    // c2.SaveAs("contour.pdf");

    double sigmas [4] = {7.79462e-02, 5.44037e-02, 3.77157e-02, 2.59455e-02};
    int values [4] = {100, 200, 400, 800};

    TGraph g;
    for (int i = 0; i < 4; i++){
        g.SetPoint(i, values[i], sigmas[i]);
    } 

    TCanvas plot ("plot", "plot");
    plot.cd();
    g.GetXaxis()->SetTitle("Number of points");
    g.GetYaxis()->SetTitle("#sigma");   
    g.SetLineColor(2);
    g.SetLineWidth(2);
    g.SetTitle("Fit on #sigma");
    g.Draw("ALP*");
    

    app.Run();

    
    // canvas.Close();

    // delete canvas;
    // delete tick;
    delete xVecPtr;
    // delete c1;
    // delete c2;

    return 0;

}
