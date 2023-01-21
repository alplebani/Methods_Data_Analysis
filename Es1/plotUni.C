// Simple ROOT macro to plot a histogram.
// Modify arguments as needed.
// To execute, type .X plotHist.C
{
  TFile* f = new TFile("simpleMC.root");
  f->ls();
  TH1D* h1 = (TH1D*)f.Get("h_Uni");
  h1->SetMinimum(0.);
  h1->SetXTitle("r");
  h1->SetYTitle("f(r)");
  h1->Draw();
}
