#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TObjArray.h>
#include <iostream>

void drawTracksFromTree(const char *fName, const char *numElec, const char *outName) {
  TFile * infile = TFile::Open(fName, "READ");
  TTree * tree = (TTree*)infile->Get("LDMX_Events");
  TCanvas * c1 = new TCanvas("c1", "plotting canvas", 600, 500);
  TH1F * myHist;
  c1->cd(); 
  int layerNum[2] = {34,20};

  std::string trackPassName = "triggerSums"; // overlay for signal 2-4e, sim for large inclusive, rereco for everything else
  std::string trackCollection = Form("@TriggerPadTracks_%s", trackPassName.c_str());

  std::string triggerSumPassName = "triggerSums"; // sim for large inclusive, triggerSums for else
  std::string triggerSumCollection;

  // for the trigger sums
  int nBarBins = 100;
  int barMin = 0;
  int barMax = 20000;
  TObjArray Hlist(0);
  

  //use this to list contents of the tree 
  tree->ls();
  tree->SetLineWidth(2);
  tree->SetLineStyle(1);

  // histogram of number of events in each track
  tree->Draw(Form("%s.size() >> htracks(6, 0, 6)", trackCollection.c_str()));
  myHist=(TH1F*)gDirectory->Get("htracks");
  myHist->SetName("tracksVsEvents");
  Hlist.Add(myHist);

  bool isFirst = true;
  bool isFirst2 = true;

  for (int n : layerNum) {
    triggerSumCollection=Form("TriggerSums%iLayers_%s", n, triggerSumPassName.c_str());
    for (int trk = 0; trk <= 5; trk++) {
      if(isFirst) {
        tree->Draw(Form( "%s.variables_.fArray[0] >> h( %i, %i, %i)" , triggerSumCollection.c_str(), nBarBins, barMin, barMax), Form("%s.size() == %i", trackCollection.c_str(), trk));
        isFirst = false;
      }
      else {
        tree->Draw(Form( "%s.variables_.fArray[0] >> h( %i, %i, %i)" , triggerSumCollection.c_str(), nBarBins, barMin, barMax),Form("%s.size() == %i", trackCollection.c_str(), trk),"SAME");
      }
      myHist=(TH1F*)gDirectory->Get("h");
      myHist->SetName(Form("TriggerSums%iLayers_%itrk", n, trk));
      Hlist.Add(myHist);
    }
    if (isFirst2) {
      tree->Draw(Form("%s.size():%s.variables_.fArray[0] >> hcompare(%i, %i, %i, 6, 0, 6)", trackCollection.c_str(), triggerSumCollection.c_str(), nBarBins, barMin, barMax));
      isFirst2 = false;
    }
    else {
      tree->Draw(Form("%s.size():%s.variables_.fArray[0] >> hcompare(%i, %i, %i, 6, 0, 6)", trackCollection.c_str(), triggerSumCollection.c_str(), nBarBins, barMin, barMax), "", "SAME");
    }
    myHist=(TH1F*)gDirectory->Get("hcompare");
    myHist->SetName(Form("tracksVsEnergy%iLayers", n));
    Hlist.Add(myHist);
  }

  cout << outName << endl;

  TFile * outfile = new TFile(outName, "RECREATE");

  Hlist.Write();
  outfile->Close();
}
