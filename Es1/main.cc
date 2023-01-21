// Program to illustrate use of ROOT random number and histogram classes
// Glen Cowan, RHUL, Physics, November 2007

#include <iostream>
#include <cmath>
#include <TH1D.h>
#include <TFile.h>
#include <TRandom3.h>
#include "TApplication.h"
#include "TCanvas.h"



using namespace std;

double pdf_saw(double x, double x_max){
  if(x > x_max or x < 0){
    return 0;
  }
  else{
    return 2*x/(x_max*x_max);
  }
}

double inverse_saw(double x, double x_max){
  return x_max*sqrt(x);
}

int main(){

  TApplication app("app",0,0);

  TFile* file = new TFile("es1.root", "recreate");

  TH1D h_1 ("h_1", "r1+r2-1",  100,  -1.0, 1.0);
  TH1D h_2 ("h_2", "r1+r2+r3+r4-2",  100,  -2.0, 2.0);
  TH1D h_3 ("h_3", "r_12 -6", 100, -6.0, 6.0);

  int seed = 12345;
  TRandom3* ran = new TRandom3(seed);

  const int numValues = 10000;
  const double xi = 1.0; 
  
  double r_1 [2];
  double r_2 [4];
  double r_3 [12];

  for (int i = 0; i < numValues; ++i){
    double r1 = ran->Rndm();
    double r2 = ran->Rndm();
    double r = r1 + r2 - 1;
    h_1.Fill(r);
    double r_h2 = 0;
    for (int j = 0; j < 4; j++){
      double temp = ran->Rndm(); 
      r_h2 += temp;      
    }
    h_2.Fill(r_h2 -2);
    double r_h3 = 0;
    for(int j = 0; j < 12; j++){
      double temp = ran->Rndm();
      r_h3 += temp;
    }
    h_3.Fill(r_h3 - 6);
  }

  TCanvas C = ("H1","H1");
  C.cd();
  h_1.SetXTitle("x");
  h_1.SetYTitle("f(x;#xi)");
  h_1.Draw();
  C.SaveAs("c.pdf");

  TCanvas C2 = ("H2","H2");
  C2.cd();
  h_2.SetXTitle("x");
  h_2.SetYTitle("f(x;#xi)");
  h_2.Draw();
  C2.SaveAs("c2.pdf");

  TCanvas C3 = ("H3","H3");
  C3.cd();
  h_3.SetXTitle("x");
  h_3.SetYTitle("f(x;#xi)");
  h_3.Draw();
  C3.SaveAs("c3.pdf");

  file->Write();
  file->Close();

  double my_x_max = 1;

  TH1F saw ("saw_tooth", "saw_tooth", 100, 0., my_x_max);
  
  for(int i = 0; i < numValues; i++){
    saw.Fill(inverse_saw(ran->Rndm(), my_x_max));
  }

  TCanvas c_saw = ("saw_tooth","saw_tooth");
  c_saw.cd();
  saw.SetXTitle("x");
  saw.SetYTitle("f(x;#xi)");
  saw.Draw();
  c_saw.SaveAs("saw.pdf");

  TH1D acc_rej ("accept_reject", "accept_reject", 100, 0., my_x_max);

  int count_in = 0;

  for (int i = 0; i < numValues; i++){
    double x = ran->Rndm();
    double y = 2 * ran->Rndm();
    if (y <= pdf_saw(x, my_x_max)){
      count_in++;
      acc_rej.Fill(x);
    }
  }

  TCanvas c_AR = ("acc_rej", "acc_rej");
  c_AR.cd();
  acc_rej.SetXTitle("x");
  acc_rej.SetYTitle("f(x;#xi)");
  acc_rej.Draw();
  c_AR.SaveAs("AR.pdf");

  cout << "Efficiency of the algorithm : " << 1.*count_in/numValues << endl;

  app.Run();

  return 0;
}
