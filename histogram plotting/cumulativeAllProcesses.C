#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TObjArray.h>
#include <THStack.h>
#include <iostream>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include <vector>
#include <string>
#include <math.h>

int getDesignatedLineColor(int k) { 
    if (k == 0) { return 9;} // inclusive (blue)
    else if (k == 1) { return 46;} // ecalPN (red)
    else { return 8;} // signal (green)
}

void cumulativeAllProcesses(const char *mult, const char *apMass)
{
    gStyle->SetOptStat(0);

    // constants
    int NUM_PROCESSES = 3;
    vector<int> layerNum = {34,20};
    char* fileName[3] = {Form("inclusive%se", mult), Form("ecalPN%se", mult), Form("signal%seAp%sGeV", mult, apMass)};
    char* label[3] = {Form("Inclusive"), Form("Ecal PN"), Form("Signal")}; 
    
    // cut-off constants
    vector<float> maxRates = {2.7e-5,1e-5,1e-6};
    vector<vector<int>> thresholdBins;
    vector<TH1 *> signalHists;
    int signalNEntries;
    bool isFirst = true;
    
    // initializations
    TCanvas* c_normal = new TCanvas("c_normal", "normal event canvas", 600, 500);
    c_normal->SetLeftMargin(1.5*c_normal->GetLeftMargin());
    TCanvas* c_log = new TCanvas("c_log", "log event canvas", 600, 500);
    c_log->SetLeftMargin(1.5*c_log->GetLeftMargin());

    // example legend entries
    TH1F* hSolid = new TH1F();
    TH1F* hDash = new TH1F();
    hDash->SetLineColor(1);
    hDash->SetLineStyle(2);
    hSolid->SetLineColor(1);
    TLegend* legend = new TLegend(60, 60, "");
    legend->AddEntry(hDash, "20 layers", "l");
    legend->AddEntry(hSolid, "34 layers", "l");

    for (int proc=0; proc < NUM_PROCESSES; proc++) { // 0 inclusive, 1 ecalPN, 2 signal
        TFile* infile = TFile::Open(Form("%s_allrunsLarge.root", fileName[proc]));
        for (unsigned int n = 0; n < layerNum.size(); n++) {
            TH1* htemp;
            if (proc == 0) { htemp = (TH1 *)infile->Get(Form("TriggerSums%iLayers_sim", layerNum[n])); } // sim if large, triggerSums if else
            else { htemp = (TH1 *)infile->Get(Form("TriggerSums%iLayers_triggerSums", layerNum[n])); }
            if (proc == 2) { signalNEntries = htemp->GetEntries(); } // change to get different layers
            htemp->Scale(1./htemp->Integral());
            TH1* hist = htemp->GetCumulative();
            hist->SetLineColor(getDesignatedLineColor(proc));
            if (layerNum[n]==20) { hist->SetLineStyle(2); }
            else { legend->AddEntry(hist, label[proc], "l"); }

            c_normal->cd();  
            hist->SetTitle(Form("Cumulative Plot: All Processes %se, Signal Mass Pt %s GeV;Summed Ecal Rec Hit Energy (MeV);Cumulative Events Below Energy",mult,apMass));
            if (isFirst) { hist->Draw(); }
            else { hist->Draw("same"); }
            // If the current process is inclusive, for each cut-off rate in the maxRates array, tempBins records the highest bin below each cut-off rate. Then that vector is added to the vector thresholdBins.
            if(proc == 0) {
                int binCount;
                vector<int> tempBins;
                for (unsigned int c = 0; c < maxRates.size(); c++) {
                    binCount=0;
                    while (hist->GetBinContent(binCount) < maxRates[c]) {
                        binCount++;
                    }
                    tempBins.push_back(binCount);
                }
                thresholdBins.push_back(tempBins);
            }
            // Signal histograms are recorded so that the data can be retrieved after the full cut-off rates are calculated in the inclusive case (to make it less convoluted).
            if (proc == 2) {
                signalHists.push_back(hist);
            }
        
            // log canvas
            c_log->cd();
            TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
            hist_log->SetTitle(Form("Cumulative Plot: All Processes %se, Signal Mass Pt %s GeV (Log Scale);Summed Ecal Rec Hit Energy (MeV);Cumulative Events Below Energy", mult, apMass));
            hist_log->SetMinimum(1);
            hist_log->SetAxisRange(1e-6,1,"y");
            hist_log->GetYaxis()->SetMoreLogLabels();
            hist_log->GetYaxis()->SetTitleOffset(2+hist->GetYaxis()->GetTitleOffset());
            if (isFirst) {
                isFirst = false;
                hist_log->Draw();
            }
            else { hist_log->Draw("same"); }
        }
    }

    TFile *outfile = new TFile(Form("cumulativeallprocesses%se%sGeV.root",mult,apMass), "RECREATE");
    // print out latex table
    FILE *latexTable = fopen(Form("cumulativeallprocesses%se%sGeV_table.tex", mult, apMass), "w");
    fprintf(latexTable,"\\documentclass[../main.tex]{subfiles}\n");
    fprintf(latexTable,"\\begin{document}\n");
    fprintf(latexTable,"\\begin{table}[h!]\n");
    fprintf(latexTable,"\\centering\n");
    fprintf(latexTable,"\\begin{tabular}{| c | c | c | c |}\n\\hline\n");
    fprintf(latexTable,"\\textbf{TS} & \\textbf{Inclusive} & \\textbf{Signal} & \\textbf{Energy Threshold}  \\\\ \\textbf{Layers} & \\textbf{Rate} & \\textbf{Rate} & \\ (GeV) \\\\ \n");
    fprintf(latexTable,"\\hline\n");
    for (unsigned int c = 0; c < thresholdBins[0].size(); c++) { // cut-off rate (corresponding to maxRates array)
        for (unsigned int n = 0; n < thresholdBins.size(); n++) { // layer
            // retrieve the rate of the signal at the cut-off rate of inclusive, and determine inclusive energy threshold
            TString currentRate = Form("%5.3g", maxRates[c]);
            float signalRate = signalHists[n]->GetBinContent(thresholdBins[n][c]);
            
            float confidenceInterval = 2.57 * sqrt(signalRate * (1 - signalRate) / (signalNEntries));
            float energyThreshold = thresholdBins[n][c] * 0.2; // DEPENDS on the bin sizes
            
            TH1F* ETHist = new TH1F(Form("energyThreshold_cutoff%s_layer%i", currentRate.Data(), layerNum[n]), Form("Energy Threshold Cutoff %s Layer %i", currentRate.Data(), layerNum[n]), 1, 0, 1);
            TH1F* CIHist = new TH1F(Form("CI_cutoff%s_layer%i", currentRate.Data(), layerNum[n]), Form("Confidence Interval Cutoff %s Layer %i", currentRate.Data(), layerNum[n]), 1, 0, 1);
            ETHist->SetBinContent(1, energyThreshold);
            CIHist->SetBinContent(1, confidenceInterval);
            ETHist->Write();
            CIHist->Write();
            if (latexTable!=NULL) {
                currentRate.ReplaceAll("e-0", " $\\times 10^{-");
                currentRate.Append("}$");
                if (n == 0) {
                    fprintf(latexTable,"%i & %s & %.2f $\\pm$ %.4f & %.1f \\\\ \n", layerNum[n], currentRate.Data(), signalRate, confidenceInterval, energyThreshold);
                }
                else {
                    fprintf(latexTable,"%i & & %.2f $\\pm$ %.4f & %.1f \\\\ \n", layerNum[n], signalRate, confidenceInterval, energyThreshold);
                }
                cout << Form("%i layers, Inclusive Rate %5.3g: Bin Found %i, Signal Rate %.2f, Energy Threshold %.1f GeV", layerNum[n], maxRates[c], thresholdBins[n][c], signalRate, energyThreshold) << endl;
            }
        }
        fprintf(latexTable, "\\hline \n");
    }
    fprintf(latexTable, "\\end{tabular}\n");
    fprintf(latexTable, Form("\\caption{Trigger Efficiency Rate %se %s GeV Mass Point}\n", mult, apMass));
    fprintf(latexTable, "\\end{table}\n");
    fprintf(latexTable, "\\end{document}");
    
    // outfile and png's with histograms
    // TFile *outfile = new TFile(Form("cumulativeallprocesses%se%sGeV.root",mult,apMass), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("cumulativeallprocesses%se%sGeV.png",mult,apMass));
    c_normal->Write();

    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("cumulativeallprocesses%se%sGeV_log.png",mult,apMass));
    c_log->Write();

    outfile->ls();

    outfile->Close();
}
