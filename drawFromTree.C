#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TObjArray.h>
#include <iostream>

void drawFromTree(const char *fName, const char *numElec, const char *outName) {
  TFile * infile = TFile::Open(fName, "READ");
  TTree * tree = (TTree*)infile->Get("LDMX_Events");
  TCanvas * c1 = new TCanvas("c1", "plotting canvas", 600, 500);
  TH1F * myHist;
  // c1->cd(); 
  // int layerNum[8] = {34,32,30,28,26,24,22,20};
  int layerNum[2] = {34,20};

  std::string passName = "sim"; // global variable to override local variable
  std::string collection;

  int nBarBins = 100;
  int barMin = 0;
  int barMax=20000;
  TObjArray Hlist(0);
  

  //use this to list contents of the tree 
  tree->ls();
  tree->SetLineWidth(2);
  tree->SetLineStyle(1);

  bool isFirst = true;
  for (int n : layerNum) {
    collection=Form("TriggerSums%iLayers_%s", n, passName.c_str());
    if(isFirst) {
      tree->Draw(Form( "%s.variables_.fArray[0] >> h( %i,%i,%i ) " , collection.c_str(), nBarBins, barMin, barMax));
    }
    else {
      tree->Draw(Form( "%s.variables_.fArray[0] >> h( %i,%i,%i ) " , collection.c_str(), nBarBins, barMin, barMax),"","SAME");
    }
    myHist=(TH1F*)gDirectory->Get("h");
    myHist->SetName(collection.c_str());
    Hlist.Add(myHist);
  }
  cout << outName << endl;

  TFile * outfile = new TFile(outName, "RECREATE");

  Hlist.Write();
  outfile->Close();
}
