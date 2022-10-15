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

int getDesignatedLineColor(int trk) {
    if (trk == 1) { return 8; }
    else if (trk == 2) { return 38; }
    else { return 9; }
}

void quickoverlaycumulativetracks(const char *process, const char *var)
{
    gStyle->SetOptStat(0);

    // constants
    int layerNum[2] = {34, 20};
    char* label[4] = {Form("1 trk"), Form("2 trk"), Form("3 trk")};

    bool isFirst = true;

    // initializations
    TCanvas* c_normal = new TCanvas("c_normal", "normal event canvas", 600, 500);
    TCanvas* c_log = new TCanvas("c_log", "log event canvas", 600, 500);
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

    TFile* infile = TFile::Open(Form("%s_allruns_%spoisson.root", process, var));
    for (int n : layerNum) {
        for (int trk = 1; trk <= 3; trk++) {
            TH1* hist = (TH1 *)infile->Get(Form("TriggerSums%iLayers_%itrk_cumulative", n, trk));
            hist->SetLineColor(getDesignatedLineColor(trk));
            if (n==20) { hist->SetLineStyle(2); }
            else { legend->AddEntry(hist, label[trk-1], "l");  }

            // normal canvas
            c_normal->cd();
            hist->SetTitle(Form("%s 1-3trk Cumulative Energy Curves, Variance = %s;Summed Ecal Rec Hit Energy (MeV);Events", process, var));
            if (isFirst) {
                hist->Draw();
            }
            hist->Draw("same");

            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetTitle(Form("%s 1-3trk Cumulative Energy Curves, Variance = %s (Log Scale);Summed Ecal Rec Hit Energy (MeV);Events", process, var));
            if (isFirst) {
                hist_log->Draw();
                isFirst = false;
            }
            hist_log->Draw("same");
        }
    }
    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("%s1-3cumulativetracks_%svar_normal.png", process, var));
    c_normal->Write();
    
    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("%s1-3cumulativetracks_%svar_log.png", process, var));
    c_log->Write();
}
