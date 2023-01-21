// A simple program to generate exponential random numbers and
// store them in a histogram; also optionally writes the individual
// values to a file.

// Glen Cowan
// RHUL Physics
// 2 December 2006

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>

#include <TFile.h>
#include <TH1D.h>
#include <TRandom3.h>
#include <TApplication.h>
#include <TCanvas.h>

using namespace std;

int main(int argc, char **argv) {

  TApplication app("app", 0, 0);

// Set up output file

  ofstream dataFile;
  dataFile.open("expData_2.txt");
  if ( dataFile.fail() ) {
    cout << "Could not open output file -- exit program" << endl;
    exit(1);
  }

// Create a TRandom object to generate random numbers uniform in ]0,1]
// Use the "Marsenne Twister" algorithm of TRandom3

  int seed = 12345;
  TRandom* ran = new TRandom3(seed);
 
//  Fill with exponential random numbers.

  const double x1 = 1.0; 
  double x2 = 5.0;
  double alpha = 0.2;
  int numVal = 0;
  cout << "Enter number of values to generate: ";
  cin >> numVal;

  TH1F histo ("histo", "histo", 20, 0, 10);

  for (int i = 0; i<numVal; ++i){
    double r = ran->Rndm();
    double x = 0.;
    if (r < alpha){
      double r1 = ran->Rndm();
      x = - x1 * log(r1); 
    }
    else {
      double r1 = ran->Rndm();
      x = - x2 * log(r1);
      
    }
    dataFile << x << endl;
    histo.Fill(x);
  }


  TCanvas c;
  c.cd();
  histo.Draw();
// Save all histograms and close up.

  dataFile.close();

  app.Run();

  return 0;

}
