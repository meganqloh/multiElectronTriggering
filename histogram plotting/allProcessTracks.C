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

int getDesignatedLineColor(int k) { 
    if (k == 0) { return 9;} // inclusive (blue)
    else if (k == 1) { return 46;} // ecalPN (red)
    else { return 8;} // signal (green)
}

void allProcessTracks(const char *mult, const char *apMass)
{
    gStyle->SetOptStat(0);

    // constants
    unsigned int NUM_PROCESSES = 3;
    int layerNum[2] = {34,20};
    char* label[3] = {Form("Inclusive"), Form("Ecal PN"), Form("Signal")};
    char* fileName[3] = {Form("inclusive%se", mult), Form("ecalPN%se", mult), Form("signal%seAp%sGeV", mult, apMass)};

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
    legend->SetFillColor(0);
    legend->SetBorderSize(0);
    legend->SetShadowColor(0);
    legend->SetFillStyle(0);
    legend->SetLineColor(0);
    legend->SetLineWidth(0);
    legend->AddEntry(hDash, "20 layers", "l");
    legend->AddEntry(hSolid, "34 layers", "l");

    for (unsigned int k = 0; k < NUM_PROCESSES; k++) { // 0 inclusive, 1 ecalPN, 2 signal
        TFile* infile = TFile::Open(Form("%s_allruns_trk.root", fileName[k]));
            TH1* hist = (TH1 *)infile->Get(Form("tracksVsEvents")); // pass name sim for large inclusive
            hist->SetLineColor(getDesignatedLineColor(k));
            legend->AddEntry(hist, label[k], "l");

            // normal canvas
            c_normal->cd();
            hist->SetTitle(Form("N Tracks vs. Events %se, Signal Mass Pt %s GeV;Summed Ecal Rec Hit Energy (MeV);Events", mult, apMass));
            if (isFirst) {
                hist->Draw();
            }
            hist->Draw("same");

            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetAxisRange(0.5, 5e7, "Y");
            hist_log->SetTitle(Form("N Tracks vs. Events %se, Signal Mass Pt %s GeV (Log Scale);Summed Ecal Rec Hit Energy (MeV);Events", mult, apMass));
            if (isFirst) {
                hist_log->Draw();
                isFirst = false;
            }
            hist_log->Draw("same");
    }
    TFile* outfile = new TFile(Form("tracks_%se%sGeV.root", mult, apMass), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("tracks_%se%sGeV_normal.png", mult, apMass));
    c_normal->Write();
    
    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("tracks_%se%sGeV_log.png", mult, apMass));
    c_log->Write();

    outfile->Close();
}
