The program simpleMC.cc is a standalone C++ program that uses ROOT
classes to book, fill and store some histograms.  To build the program, 
download all the files to a separate directory. Then build
the program as usual by typing

gmake (or make) 

Run the program with

./simpleMC

which should produce the output file simpleMC.root.  To view the
histograms in this file, use the program root (see cern.root.ch).

A minimal root session to view the histograms could be:

> root                         <--- prompt is ">", you type "root".
 ------------------------------------------------------------
  | Welcome to ROOT 6.15/01                  https://root.cern |
  |                               (c) 1995-2018, The ROOT Team |
  | Built for macosx64 on Oct 31 2018, 23:05:33                |
  | From heads/master@v6-13-04-1957-g8914441b53                |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------

root [0] TFile* f = new TFile("simpleMC.root");
root [1] f->ls();
TFile**         simpleMC.root
 TFile*         simpleMC.root
  KEY: TH1D     h_Uni;1 uniform random numbers
  KEY: TH1D     h_Exp;1 exponential random numbers
root [2] h_Uni->Draw();
<TCanvas::MakeDefCanvas>: created default TCanvas with name c1
root [3] h_Exp->Draw();
root [4] .q

Another way to view the histograms is to use a TBrowser.  To do this,
first start root, read in the file and create the TBRowser as follows:

> root 
 ------------------------------------------------------------
  | Welcome to ROOT 6.15/01                  https://root.cern |
  |                               (c) 1995-2018, The ROOT Team |
  | Built for macosx64 on Oct 31 2018, 23:05:33                |
  | From heads/master@v6-13-04-1957-g8914441b53                |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------
root [0] TFile* f = new TFile("simpleMC.root");
TBrowser b;

A Window will pop up that contains folder icons, one of which on the
right-hand side will be named "ROOT Files".  Open (double-click)
this and you will see an icon representing your root file (simpleMC.root).
Open this and you will see icons representing all of the histograms.
By double clicking these you will get a plot of the histogram.  This
is a handy way of taking a quick look at the histograms in a file.

A better way to make plots is to write a short macro that will 
execute the root commands for you.  Two examples are in plotHist.C
and plot2Hist.C.  To execute a macro, you type (at the root prompt)
.X macro-name e.g.,

.X plotHist.C

To store the plot as a postscript file, go to the graphics window (the
"root canvas") and select File -> Save As -> c1.eps.  For plots to
include in PowerPoint, Word, etc., I find that gif format works best.


Nicola Neri
adapted from exercises of
Glen Cowan
RHUL Physics
November 2007

