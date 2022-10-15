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

void fixedNSimElectronsSignal(const char *nSimElectron, const char *apMass)
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

    TFile* infile = TFile::Open(Form("signal%seAp%sGeV_allruns_trk.root", nSimElectron, apMass));
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
            hist->SetLineColor(getDesignatedLineColor(k));
            if (n==20) { hist->SetLineStyle(2); }
            else if (hist->GetEntries() > 0) { legend->AddEntry(hist, label[k], "l"); }

            // normal canvas
            hist->SetMaximum(setmax);

            c_normal->cd();
            hist->SetTitle(Form("Signal %s GeV %s Simulated Electron Overlay;Summed Ecal Rec Hit Energy (MeV);Events", apMass, nSimElectron));
            if (isFirst) {
                hist->Draw();
            }
            hist->Draw("same");

            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetTitle(Form("Signal %s GeV %s Simulated Electron Overlay (Log Scale);Summed Ecal Rec Hit Energy (MeV);Events", apMass, nSimElectron));
            if (isFirst) {
                hist_log->Draw();
            }
            hist_log->Draw("same");
            
            // unit area canvas
            c_unitarea->cd();
            TH1 * hist_unitarea = (TH1*)(hist->Clone("hist_unitarea"));
            hist_unitarea->Scale(1/hist->Integral());
            hist_unitarea->SetTitle(Form("Signal %s GeV %s Simulated Electron Overlay (Unit Area);Summed Ecal Rec Hit Energy (MeV);Events", apMass, nSimElectron));
            if (isFirst) {
                isFirst = false;
                hist_unitarea->Draw("h");
            }
            hist_unitarea->Draw("hsame");
        }
    }
    TFile* outfile = new TFile(Form("signal%sAp%sGeVsimE.root", nSimElectron, apMass), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("signal%sAp%sGeVSimE_normal.png", nSimElectron, apMass));
    c_normal->Write();
    
    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("signal%sAp%sGeVSimE_log.png", nSimElectron, apMass));
    c_log->Write();

    c_unitarea->cd();
    legend->Draw();
    c_unitarea->SaveAs(Form("signal%sAp%sGeVSimE_unitarea.png", nSimElectron, apMass));
    c_unitarea->Write();

    outfile->Close();
}
