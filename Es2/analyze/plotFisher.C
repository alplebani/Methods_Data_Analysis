{

  // Set some defaults
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders
  gStyle->SetOptStat(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFont(42, "h");        // for histogram title
  gStyle->SetTitleFont(42, "xyz");      // for axis labels
  gStyle->SetLabelFont(42, "xyz");      // for axis values
  gStyle->SetTitleSize(0.04, "h");
  gStyle->SetTitleSize(0.04, "xyz");
  gStyle->SetPadLeftMargin(0.17);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.17);
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(0.55);

  gROOT->ForceStyle();

  // Open the file and list its contents, get the Ttrees
  TFile* f = new TFile("analysis.root");
  f->ls();
  TH1D* hSig = (TH1D*)f->Get("hSig");
  TH1D* hBkg = (TH1D*)f->Get("hBkg");

  // Create canvas and use implicitly associated 
  // histogram to set min/max values and set axis labels.
  TCanvas* c = new TCanvas("c", "Canvas 1", 10, 10, 600, 600);
  
  hSig->SetNdivisions(505, "x");
  hSig->SetNdivisions(505, "y");
  hSig->SetXTitle("t");
  hSig->SetYTitle("entries");
  hSig->SetTitle("Fisher");
  hSig->SetLineStyle(1);
  hSig->SetLineWidth(3);
  hSig->SetLineColor(kBlue);
  hBkg->SetLineStyle(2);
  hBkg->SetLineWidth(3);
  hBkg->SetLineColor(kRed);
  TAxis* xa = hSig->GetXaxis();
  TAxis* ya = hSig->GetYaxis();
  xa->SetTitleOffset(1.2);    //  factor multiplies default offset
  ya->SetTitleOffset(1.6);


  double maxSig = hSig->GetMaximum();
  double maxBkg = hBkg->GetMaximum();
  double maxVal = std::max(maxSig, maxBkg) * 1.4;
  hSig->SetMaximum(maxVal);
  double xMin = -4.;                 // set as appropriate
  double xMax = 4.;
  int binLo = hSig->FindBin(xMin);
  int binHi = hSig->FindBin(xMax);
  hSig->GetXaxis()->SetRange(binLo, binHi);
  hSig->DrawCopy();
  hBkg->DrawCopy("same");

  TLegend* leg = new TLegend(0.2, 0.72, 0.6, 0.88); // x1, y1, x2, y2
  leg->SetTextSize(0.04);
  leg->SetTextFont(42);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->AddEntry(hSig, "  signal", "l");
  leg->AddEntry(hBkg, "  background", "l");
  leg->Draw();


}
