// Macro to make scatter plots of the generated data.

{

  // Set some defaults
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders
  gStyle->SetOptStat(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFont(42, "h");        // for histogram title
  gStyle->SetLabelFont(42, "xyz");      // for axis labels
  gStyle->SetPadLeftMargin(0.17);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.17);
  gStyle->SetTitleX(0.15);              // for histogram title
  gStyle->SetTitleY(1.0);

  // Open the file and list its contents, get the Ttrees
  TFile* f = new TFile("./testData.root");
  f->ls();
  TTree*sig = (TTree*)f->Get("sig");
  TTree*bkg = (TTree*)f->Get("bkg");

  sig->SetMarkerColor(kBlue);
  sig->SetMarkerStyle(20);        // full circle
  sig->SetMarkerSize(2);
  bkg->SetMarkerColor(kRed);
  bkg->SetMarkerStyle(22);        // full triangle up
  bkg->SetMarkerSize(2);

  // Create canvas, divide into 2 by 2, and use implicitly associated 
  // histogram to set min/max values and set axis labels.
  TCanvas *c1 = new TCanvas("c1", "Canvas 1", 10, 10, 600, 600);
  c1->Divide(2,2);
  TH2F* hframe = new TH2F("hframe", "", 1, -2.5, 4., 1, -2.5, 4.);
  hframe->SetXTitle("x");
  hframe->SetYTitle("y");

  // In each pad make plot
  c1->cd(1);
  hframe->SetTitle("no cut on z");
  hframe->DrawCopy();
  sig->Draw("y:x", "", "same");
  bkg->Draw("y:x", "", "same");

  c1->cd(2);
  hframe->SetTitle("z<0.75");
  hframe->DrawCopy();
  sig->Draw("y:x", "z<0.75", "same");
  bkg->Draw("y:x", "z<0.75", "same");

  c1->cd(3);
  hframe->SetTitle("z<0.5");
  hframe->DrawCopy();
  sig->Draw("y:x", "z<0.5", "same");
  bkg->Draw("y:x", "z<0.5", "same");

  c1->cd(4);
  hframe->SetTitle("z<0.25");
  hframe->DrawCopy();
  sig->Draw("y:x", "z<0.25", "same");
  bkg->Draw("y:x", "z<0.25", "same");

}
