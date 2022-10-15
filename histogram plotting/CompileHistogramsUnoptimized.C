#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TObjArray.h>
#include <THStack.h>
#include <iostream>

void compileHistogramsUnoptimized(const char *fName1, const char *fName2, const char *fName3, const char *fName4)
{
    int layerNum[2] = {34,20};
    bool logY = false;
    bool unitArea = false;
    // int layerNum = 34;
    // can optimize this with naming convention used?
    TFile *infile1 = TFile::Open(fName1, "READ");
    TFile *infile2 = TFile::Open(fName2, "READ");
    TFile *infile3 = TFile::Open(fName3, "READ");
    TFile *infile4 = TFile::Open(fName4, "READ");

    TCanvas *c1 = new TCanvas("c1", "plotting canvas", 600, 500);
    THStack *ths = new THStack("hs", Form("Trigger Sum %i and %i Layers 1e-4e", layerNum[0], layerNum[1]));
    // THStack *ths = new THStack("hs"), Form("Trigger Sum %i Layers 1e-4e", layerNum)

    for (int n : layerNum)
    {
        TH1F *h1 = (TH1F *)infile1->Get(Form("TriggerSums%iLayers_triggerSums", n));
        h1->SetName("1e Energy Sums");
        TH1F *h2 = (TH1F *)infile2->Get(Form("TriggerSums%iLayers_triggerSums", n));
        h2->SetName("2e Energy Sums");
        TH1F *h3 = (TH1F *)infile3->Get(Form("TriggerSums%iLayers_triggerSums", n));
        h3->SetName("3e Energy Sums");
        TH1F *h4 = (TH1F *)infile4->Get(Form("TriggerSums%iLayers_triggerSums", n));
        h4->SetName("4e Energy Sums");
        h4->SetLineWidth(2);
        if (n==20) {
            h1->SetLineStyle(2);
            h2->SetLineStyle(2);
            h3->SetLineStyle(2);
            h4->SetLineStyle(2);
        }
        if (unitArea) {
            h1->Scale(1/h1->Integral());
            h2->Scale(1/h2->Integral());
            h3->Scale(1/h3->Integral());
            h4->Scale(1/h4->Integral());
        }
        ths->Add(h1);
        ths->Add(h2);
        ths->Add(h3);
        ths->Add(h4);
    }

    // TFile *outfile = new TFile(Form("compiledHistogramsLayer%i.root", layerNum), "RECREATE");
    TFile *outfile = new TFile(Form("compiledHistogramsLayer%iand%i.root", layerNum[0], layerNum[1]), "RECREATE");
    ths->Draw("nostack");
    ths->GetXaxis()->SetTitle("Summed Ecal Rec Hit Energy (MeV)");
    ths->GetYaxis()->SetTitle("Events");
    ths->Write();
    if (logY) { 
        ths->GetYaxis()->SetTitle("Log Events");
        outfile = new TFile(Form("compiledHistogramsLayer%iand%iLog.root", layerNum[0], layerNum[1]), "RECREATE");
        c1->SetLogy();
        c1->Write();
    }
    if (unitArea) {
        ths->GetYaxis()->SetTitle("Unit Area of Events");
        outfile = new TFile(Form("compiledHistogramsLayer%iand%iUnitArea.root", layerNum[0], layerNum[1]), "RECREATE");
        ths->Write();
    }
    outfile->Close();
}
