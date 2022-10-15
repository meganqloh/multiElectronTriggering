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
        else if (k == 3) { return 44;}
        else if (k == 4) { return 42;}
        else { return 41;}
    }
    else { // inclusive colors (blue)
        if (k == 0) { return 4; }
        else if (k == 1) { return 9;}
        else if (k == 2) { return 38;}
        else if (k == 3) { return 39;}
        else if (k == 4) { return 40;}
        else { return 15;}
    }
}

void fixedNSimElectrons(const char *process, const char *nSimElectron)
{
    gStyle->SetOptStat(0);

    // constants
    int layerNum[2] = {34, 20};
    char* label[6] = {Form("0 tracks"), Form("1 track"), Form("2 tracks"), Form("3 tracks"), Form("4 tracks"), Form("5 tracks")};
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

    TFile* infile = TFile::Open(Form("%s%se_allruns_trk.root", process, nSimElectron));
    for (int k = 0; k <= 5; k++) {
        TH1* hist = (TH1 *)infile->Get(Form("TriggerSums34Layers_%itrk", k));
        int currentmax = hist->GetMaximum();
        if (currentmax > setmax) {
            setmax = currentmax * 1.1;
            cout << setmax << endl;
        }
    }

    for (int k = 0; k <= 5; k++) { // k is ntracks
        for (int n : layerNum) {
            TH1* hist = (TH1 *)infile->Get(Form("TriggerSums%iLayers_%itrk", n, k)); // pass name sim for 1000 event inclusive, triggerSums for other
            hist->SetLineColor(getDesignatedLineColor(k, process));
            if (n==20) { hist->SetLineStyle(2); }
            else if (hist->GetEntries() > 0) { legend->AddEntry(hist->GetName(), label[k], "l"); }

            // normal canvas
            hist->SetMaximum(setmax);

            c_normal->cd();
            hist->SetTitle(Form("%s %s Simulated Electron Overlay;Summed Ecal Rec Hit Energy (MeV);Events", process, nSimElectron));
            if (isFirst) {
                hist->Draw();
            }
            hist->Draw("same");

            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetTitle(Form("%s %s Simulated Electron Overlay (Log Scale);Summed Ecal Rec Hit Energy (MeV);Events", process, nSimElectron));
            if (isFirst) {
                hist_log->Draw();
            }
            hist_log->Draw("same");
            
            // unit area canvas
            c_unitarea->cd();
            TH1 * hist_unitarea = (TH1*)(hist->Clone("hist_unitarea"));
            hist_unitarea->Scale(1/hist->Integral());
            hist_unitarea->SetTitle(Form("%s %s Simulated Electron Overlay (Unit Area);Summed Ecal Rec Hit Energy (MeV);Events", process, nSimElectron));
            if (isFirst) {
                isFirst = false;
                hist_unitarea->Draw("h");
            }
            hist_unitarea->Draw("hsame");
        }
    }
    TFile* outfile = new TFile(Form("%s%ssimE.root", process, nSimElectron), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("%s%sSimE_normal.png", process, nSimElectron));
    c_normal->Write();
    
    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("%s%sSimE_log.png", process, nSimElectron));
    c_log->Write();

    c_unitarea->cd();
    legend->Draw();
    c_unitarea->SaveAs(Form("%s%sSimE_unitarea.png", process, nSimElectron));
    c_unitarea->Write();

    outfile->Close();
}
