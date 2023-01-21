// Simple ROOT macro to plot a pair of histogram.
// Modify arguments as needed.
// To execute, type .X plot2Hist.C
{

  // Set some defaults
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  // Open the file and list its contents
  TFile* f = new TFile("simpleMC.root");
  f->ls();

  // Get a couple of histograms
  TH1D* h1 = (TH1D*)f.Get("h_Uni");
  TH1D* h2 = (TH1D*)f.Get("h_Exp");

  // Create a canvas, divide into two windows, 
  TCanvas *c1 = new TCanvas("c1", "Canvas 1", 400, 10, 600, 700);
  c1->Divide(1,2);

  // Plot the uniform numbers in upper plot
  c1->cd(1);
  h1->SetMinimum(0.0);
  h1->SetMaximum(150.);  // use default or set as appropriate     
  h1->SetXTitle("r");
  h1->SetYTitle("f(r)");
  h1->Draw();

  // Plot the exponential numbers in lower plot
  c1->cd(2);
  h2->SetMinimum(0.0);
  h2->SetMaximum(500.);  // use default or set as appropriate
  h2->SetXTitle("x");
  h2->SetYTitle("f(x;#xi)");
  h2->Draw();

}
