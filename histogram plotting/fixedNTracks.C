#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TObjArray.h>
#include <THStack.h>
#include <iostream>
#include <TLegend.h>
#include <TStyle.h>
#include <vector>
#include <string>

int getDesignatedLineColor(int k, const char *process) {
    if (strcmp(process, "ecalPN") == 0) { // ecalPN colors (red)
        if (k == 0) { return 2; }
        else if (k == 1) { return 46;}
        else if (k == 2) { return 45;}
        else { return 44;}
    }
    else { // inclusive colors (blue)
        if (k == 0) { return 9; }
        else if (k == 1) { return 39;}
        else if (k == 2) { return 40;}
        else { return 15;}
    }
}

void fixedNTracks(const char *process, const char *nTracks)
{
    gStyle->SetOptStat(0);

    // constants
    int layerNum[2] = {34, 20};
    char* label[4] = {Form("1 e"), Form("2 e"), Form("3 e"), Form("4 e")};
    int setmax = 0;

    bool isFirst = true;

    // initializations
    TCanvas* c_normal = new TCanvas("c_normal", "normal event canvas", 600, 500);
    TCanvas* c_log = new TCanvas("c_log", "log event canvas", 600, 500);
    TCanvas* c_unitarea = new TCanvas("c_unitarea", "unit area canvas", 600, 500);
    c_normal->SetLeftMargin(1.5*c_normal->GetLeftMargin());

    // example legend entries
    TH1F* hSolid = new TH1F();
    TH1F* hDash = new TH1F();
    hDash->SetLineColor(1);
    hDash->SetLineStyle(2);
    hSolid->SetLineColor(1);
    TLegend* legend = new TLegend();
    legend->AddEntry(hDash, "20 layers", "l");
    legend->AddEntry(hSolid, "34 layers", "l");

    for (int k = 1; k <= 4; k++) {
        TFile* infile = TFile::Open(Form("%s%de_allruns_trk.root", process, k));
        TH1* hist = (TH1 *)infile->Get(Form("TriggerSums34Layers_%strk", nTracks));
        int currentmax = hist->GetMaximum();
        if (currentmax > setmax) {
            setmax = currentmax * 1.1;
        }
    }
    for (int k = 1; k <= 4; k++) { // k is n electrons
        TFile* infile = TFile::Open(Form("%s%de_allruns_trk.root", process, k));
        for (int n : layerNum) {
            TH1* hist = (TH1 *)infile->Get(Form("TriggerSums%iLayers_%strk", n, nTracks)); // pass name sim for 1000 event inclusive, triggerSums for other
            hist->SetLineColor(getDesignatedLineColor(k, process));
            if (n==20) { hist->SetLineStyle(2); }
            else if (hist->GetEntries() > 0) { legend->AddEntry(hist, label[k-1], "l");  }
            // normal canvas
            c_normal->cd();
            hist->SetTitle(Form("%s %s Tracks Overlay;Summed Ecal Rec Hit Energy (MeV);Events", process, nTracks));
            if (isFirst) {
                hist->Draw();
                hist->SetMaximum(setmax);
            }
            hist->Draw("same");

            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetTitle(Form("%s %s Tracks Overlay (Log Scale);Summed Ecal Rec Hit Energy (MeV);Events", process, nTracks));
            if (isFirst) {
                hist_log->Draw();
                isFirst = false;
            }
            hist_log->Draw("same");
        }
    }
    TFile* outfile = new TFile(Form("%s%stracks.root", process, nTracks), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("%s%stracks_normal.png", process, nTracks));
    c_normal->Write();
    
    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("%s%stracks_log.png", process, nTracks));
    c_log->Write();

    outfile->Close();
}
