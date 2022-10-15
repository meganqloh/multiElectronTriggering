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

void fixedProcess(const char *process)
{
    gStyle->SetOptStat(0);

    // constants
    int layerNum[2] = {34, 20};
    char* label[4] = {Form("1 electron"), Form("2 electrons"), Form("3 electrons"), Form("4 electrons")};

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
    TLegend* legend = new TLegend(60,60,"");
    legend->AddEntry(hDash, "20 layers", "l");
    legend->AddEntry(hSolid, "34 layers", "l");

for (unsigned int k = 0; k < 4; k++) { // k is multiplicity - 1
        TFile* infile = TFile::Open(Form("%s%ie_allrunsLarge.root", process, k+1));
        for (int n : layerNum) {
            TH1* hist = (TH1 *)infile->Get(Form("TriggerSums%iLayers_sim", n)); // pass name sim for 1000 event inclusive, triggerSums for other
            hist->SetLineColor(getDesignatedLineColor(k, process));
            if (n==20) { hist->SetLineStyle(2); }
            else { legend->AddEntry(hist, label[k], "l"); }

            // normal canvas
            c_normal->cd();
            hist->SetTitle(Form("%s Overlay;Summed Ecal Rec Hit Energy (MeV);Events", process));
            if (isFirst) {
                hist->Draw();
            }
            hist->Draw("same");

            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetTitle(Form("%s Overlay (Log Scale);Summed Ecal Rec Hit Energy (MeV);Events", process));
            if (isFirst) {
                hist_log->Draw();
            }
            hist_log->Draw("same");
            
            // unit area canvas
            c_unitarea->cd();
            TH1 * hist_unitarea = (TH1*)(hist->Clone("hist_unitarea"));
            hist_unitarea->Scale(1/hist->Integral());
            hist_unitarea->SetTitle(Form("%s Overlay (Unit Area);Summed Ecal Rec Hit Energy (MeV);Events", process));
            if (isFirst) {
                isFirst = false;
                hist_unitarea->Draw("h");
            }
            hist_unitarea->Draw("hsame");
        }
    }
    TFile* outfile = new TFile(Form("large_%s.root", process), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("large_%s_normal.png", process));
    c_normal->Write();
    
    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("large_%s_log.png", process));
    c_log->Write();

    c_unitarea->cd();
    legend->Draw();
    c_unitarea->SaveAs(Form("large_%s_unitarea.png", process));
    c_unitarea->Write();

    outfile->Close();
}
