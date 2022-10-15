#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>



void drawFromTree(const char *fName){

  TFile * infile = TFile::Open( fName, "READ");

  TTree * tree = (TTree*)infile->Get("LDMX_Events");


  TCanvas * c1 = new TCanvas("c1", "plotting canvas", 600, 500);
  c1->cd();

  std::string passName = "sim";
  std::string hitCollection="trigScintDigisTag_"+passName;

  int nBarBins = 50;
  int barMin = 0;
  int barMax=50;

  //use this to list contents of the tree 
  tree->ls();

  //example: draw the bar IDs that have hits, in all events 
  tree->Draw(Form( "%s.barID_ >> h( %i,%i,%i ) " , hitCollection.c_str(), nBarBins, barMin, barMax));
  // you can also add cuts as a second arg e.g. "EventHeader.eventNumber < 100"  
  // then to store the histogram, you can access it by name from temporary memory with TH1F * myHist = (TH1F*)gDirectory->Get("h"); 
  // and then manipulate/store it as you wish.

  //to do more advanced stuff than drawing (for instance, manipulating data), you'd need to define the variables you're interested in as variables in this macro (under any name you choose), and connect to a TBranch on the tree with a name and type. There are plenty of tree reading examples in ROOT tutorials or question fora. 

  c1->SaveAs("barIDs.png");




}
