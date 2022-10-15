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

int getDesignatedLineColor(const char *process) {
    return 8; // green
}

void cumulativeTracksSignal(const char *trk, const char *apMass, const char *variance) // mult is the simulated multiplicity, track is the trk being measured - energy threshold is based off of the trk
{
    gStyle->SetOptStat(0);

    // constants
    int NUM_PROCESSES = 3;
    vector<int> layerNum = {34,20};
    
    // cut-off constants
    vector<float> maxRates = {2.7e-5,1e-5,1e-6};
    bool isFirst = true;
    
    // initializations
    TCanvas* c_normal = new TCanvas("c_normal", "normal event canvas", 600, 500);
    c_normal->SetLeftMargin(1.5*c_normal->GetLeftMargin());
    TCanvas* c_log = new TCanvas("c_log", "log event canvas", 600, 500);
    c_log->SetLeftMargin(1.5*c_log->GetLeftMargin());

    // example legend entries
    TH1F* hSolid = new TH1F();;
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

    FILE *latexTable = fopen(Form("cumulativetracks_signal_%strk_%sGeV_%svar.tex", trk, apMass, variance), "w");
    fprintf(latexTable,"\\documentclass[../main.tex]{subfiles}\n");
    fprintf(latexTable,"\\begin{document}\n");
    fprintf(latexTable,"\\begin{table}[h!]\n");
    fprintf(latexTable,"\\centering\n");
    fprintf(latexTable,"\\begin{tabular}{| c | c | c | c |}\n\\hline\n");
    fprintf(latexTable,"\\textbf{TS} & \\textbf{Inclusive} & \\textbf{Track} & \\textbf{Energy Threshold}  \\\\ \\textbf{Layers} & \\textbf{Rate} & \\textbf{Rate} & \\ (GeV) \\\\ \n");
    fprintf(latexTable,"\\hline\n");
    TFile* infile = TFile::Open(Form("signalAp%sGeV_allruns_%spoisson.root", apMass, variance));
    TFile* energyThresholdFile = TFile::Open(Form("cumulativeallprocesses%se%sGeV.root", trk, apMass));
    for (unsigned int n = 0; n < layerNum.size(); n++) {
        TH1* hist = (TH1 *)infile->Get(Form("TriggerSums%iLayers_%strk_cumulative", layerNum[n], trk));
        cout << hist->GetEntries() << endl;
        hist->SetLineColor(8);
        if (layerNum[n]==20) { hist->SetLineStyle(2); }

        c_normal->cd();  
        hist->SetTitle(Form("Signal %s GeV Cumulative Plot %s Track with %s Variance;Summed Ecal Rec Hit Energy (MeV);Cumulative Events Below Energy", apMass, trk, variance));
        if (isFirst) { hist->Draw(); }
        else { hist->Draw("same"); }

        // log canvas
        c_log->cd();
        TH1* hist_log = (TH1*)(hist->Clone("hist_log"));
        hist_log->SetTitle(Form("Signal %s GeV Cumulative Plot %s Track with %s Variance (Log);Summed Ecal Rec Hit Energy (MeV);Cumulative Events Below Energy", apMass, trk, variance));
        hist_log->SetMinimum(1);
        hist_log->SetAxisRange(1e-6,1,"y");
        hist_log->GetYaxis()->SetMoreLogLabels();
        hist_log->GetYaxis()->SetTitleOffset(2+hist->GetYaxis()->GetTitleOffset());
        if (isFirst) {
            isFirst = false;
            hist_log->Draw();
        }
        else { hist_log->Draw("same"); }
        
        for (unsigned int c = 0; c < maxRates.size(); c++) { // cut-off rate (corresponding to maxRates array)
            TString currentRate = Form("%5.3g", maxRates[c]);
            printf("data");
            printf(currentRate.Data());
            TH1F* ETHist = (TH1F*) energyThresholdFile->Get(Form("energyThreshold_cutoff%s_layer%i", currentRate.Data(), layerNum[n]));
            float energyThreshold = ETHist->GetBinContent(1);
            cout << energyThreshold << endl;
            float trackRate = hist->GetBinContent((int) (energyThreshold * 5));
            currentRate.ReplaceAll("e-0", " $\\times 10^{-");
            currentRate.Append("}$");
            if (c == 1) {
                fprintf(latexTable,"%i & %s & %.4f & %.1f \\\\ \n", layerNum[n], currentRate.Data(), trackRate, energyThreshold);
            }
            else {
                fprintf(latexTable,"& %s & %.4f & %.1f \\\\ \n", currentRate.Data(), trackRate, energyThreshold);
            }
            if (c == 2) {
                fprintf(latexTable, "\\hline \n");
            }
            cout << "for layer " << layerNum[n] << " current rate " << currentRate.Data() << " track rate is " << trackRate << endl;
        }
    }

    TFile *outfile = new TFile(Form("cumulativetracks_signal_%strk_%sGeV_%spoisson.root", trk, apMass, variance), "RECREATE");
    
    fprintf(latexTable, "\\end{tabular}\n");
    fprintf(latexTable, Form("\\caption{Signal %s GeV %s Track Rate, $\\lambda$ = %s}\n", apMass, trk, variance));
    fprintf(latexTable, "\\end{table}\n");
    fprintf(latexTable, "\\end{document}");
    
    // outfile and png's with histograms
    // TFile *outfile = new TFile(Form("cumulativeallprocesses%se%sGeV.root",mult,apMass), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("cumulativetracks_signal_%strk_%sGeV_%spoisson_normal.png", trk, apMass, variance));
    c_normal->Write();

    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("cumulativetracks_signal_%strk_%sGeV_%spoisson_log.png", trk, apMass, variance));
    c_log->Write();

    outfile->Close();
}
