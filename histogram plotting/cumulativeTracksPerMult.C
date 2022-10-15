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
    if (strcmp(process, "ecalPN") == 0) { // ecalPN colors (red)
        return 2;
    }
    else {
        return 9;
    }
}

void cumulativeTracksPerMult(const char *process, const char *mult, const char *trk, const char *apMass) // mult is the simulated multiplicity, track is the trk being measured - energy threshold is based off of the trk
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

    FILE *latexTable = fopen(Form("cumulativetracks_%s_%ssim_%strk_%sGeV.tex", process, mult, trk, apMass), "w");
    fprintf(latexTable,"\\documentclass[../main.tex]{subfiles}\n");
    fprintf(latexTable,"\\begin{document}\n");
    fprintf(latexTable,"\\centering\n");
    fprintf(latexTable, Form("\\caption{%s Track Rate From %s Sim e compared to %s GeV Mass Point Cut Off}\n", trk, mult, apMass));
    fprintf(latexTable,"\\begin{center}\n");
    fprintf(latexTable,"\\begin{tabular}{ c | c | c | c }\n");
    fprintf(latexTable,"\\textbf{TS} & \\textbf{Inclusive} & \\textbf{Track} & \\textbf{Energy Threshold} \\\\ \\textbf{Layers} & \\textbf{Rate} & \\textbf{Rate} & \\ (GeV) \\\\ \n");
    fprintf(latexTable,"\\hline\n");

    
    TFile* infile = TFile::Open(Form("%s%se_allruns_trk.root", process, mult));
    TFile* energyThresholdFile = TFile::Open(Form("cumulativeallprocesses%se%sGeV.root", trk, apMass));
    for (unsigned int n = 0; n < layerNum.size(); n++) {
        TH1* htemp;
        htemp = (TH1 *)infile->Get(Form("TriggerSums%iLayers_%strk", layerNum[n], trk));
        htemp->Scale(1./htemp->Integral());
        TH1* hist = htemp->GetCumulative();
        hist->SetLineColor(getDesignatedLineColor(process));
        if (layerNum[n]==20) { hist->SetLineStyle(2); }

        c_normal->cd();  
        hist->SetTitle(Form("Cumulative Plot %s Track;Summed Ecal Rec Hit Energy (MeV);Cumulative Events Below Energy", trk));
        if (isFirst) { hist->Draw(); }
        else { hist->Draw("same"); }

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
        
        for (unsigned int c = 0; c < maxRates.size(); c++) { // cut-off rate (corresponding to maxRates array)
            TString currentRate = Form("%5.3g", maxRates[c]);
            TH1F* ETHist = (TH1F*) energyThresholdFile->Get(Form("energyThreshold_cutoff%s_layer%i", currentRate.Data(), layerNum[n]));
            float energyThreshold = ETHist->GetBinContent(1);
            cout << energyThreshold << endl;
            float trackRate = hist->GetBinContent((int) (energyThreshold * 5));
            fprintf(latexTable,"%i & %s & %.2f & %.2f \\\\ \n", layerNum[n], currentRate.Data(), trackRate, energyThreshold);
            cout << "for layer " << layerNum[n] << " current rate " << currentRate.Data() << " track rate is " << trackRate << endl;
        }
    }

    TFile *outfile = new TFile(Form("cumulativetracks_%s_%ssim_%strk_%sGeV.root", process, mult, trk, apMass), "RECREATE");
    
    fprintf(latexTable, "\\hline \n");
    fprintf(latexTable, "\\end{tabular}\n");
    fprintf(latexTable,"\\end{center}\n");
    fprintf(latexTable, "\\end{document}");
    
    // outfile and png's with histograms
    // TFile *outfile = new TFile(Form("cumulativeallprocesses%se%sGeV.root",mult,apMass), "RECREATE");

    c_normal->cd();
    legend->Draw();
    c_normal->SaveAs(Form("cumulativetracks_%s_%ssim_%strk_%sGeV_normal.png", process, mult, trk, apMass));
    c_normal->Write();

    c_log->cd();
    c_log->SetLogy();
    legend->Draw();
    c_log->SaveAs(Form("cumulativetracks_%s_%ssim_%strk_%sGeV_log.png", process, mult, trk, apMass));
    c_log->Write();

    outfile->Close();
}
