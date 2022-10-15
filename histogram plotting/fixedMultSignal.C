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
    if (k == 0) { return 46;}
    else if (k == 1) { return 42;}
    else if (k == 2) { return 30;}
    else { return 38;}
}

void fixedMultSignal(const char *mult) { 
    gStyle->SetOptStat(0);

    // constants
    int layerNum[2] = {34, 20};
    char* label[4] = {Form("0.001 GeV"), Form("0.01 GeV"), Form("0.1 GeV"), Form("1 GeV")};
    char* apMasses[4] = {Form("1"), Form("0.1"), Form("0.01"), Form("0.001")};

    bool isFirst = true;

    // initialize canvases, infiles, outfiles
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

    // k is number of mass points
    for (unsigned int k = 0; k < 4; k++) {
        TFile* infile = TFile::Open(Form("signal%seAp%sGeV_allrunsLayer0.root", mult, apMasses[k]));
        for (int n : layerNum) {
            TH1* hist = (TH1 *)infile->Get(Form("TriggerSums%iLayers_triggerSums", n));
            hist->SetLineColor(getDesignatedLineColor(k));
            if (n==20) { hist->SetLineStyle(2); }
            else { legend->AddEntry(hist, label[k], "l"); }

            // normal canvas
            c_normal->cd();
            hist->SetTitle(Form("%se Signal Overlay of %s GeV;Summed Ecal Rec Hit Energy (MeV);Events", mult, apMasses[k]));
            if (isFirst) {
                hist->Draw();
            }
            hist->Draw("same");

            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetTitle(Form("%se Signal Overlay of %s GeV (Log Scale);Summed Ecal Rec Hit Energy (MeV);Events", mult, apMasses[k]));
            if (isFirst) {
                hist_log->Draw();
            }
            hist_log->Draw("same");
            
            // unit area canvas
            c_unitarea->cd();
            TH1 * hist_unitarea = (TH1*)(hist->Clone("hist_unitarea"));
            hist_unitarea->Scale(1/hist->Integral());
            hist_unitarea->SetTitle(Form("%se Signal Overlay of %s GeV (Unit Area);Summed Ecal Rec Hit Energy (MeV);Events", mult, apMasses[k]));
            if (isFirst) {
                isFirst = false;
                hist_unitarea->Draw("h");
            }
            hist_unitarea->Draw("hsame");
        }
    }
    TFile* outfile = new TFile(Form("layer0_signal%se.root", mult), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("layer0_signal%se_normal.png", mult));
    c_normal->Write();
    
    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("layer0_signal%se_log.png", mult));
    c_log->Write();

    c_unitarea->cd();
    legend->Draw();
    c_unitarea->SaveAs(Form("layer0_signal%se_unitarea.png", mult));
    c_unitarea->Write();

    outfile->Close();
}
