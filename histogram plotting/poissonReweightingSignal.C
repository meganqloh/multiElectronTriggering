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

long factorial(int n)
{
  if (n == 0)
    return 1;
  else
    return (n*factorial(n-1));
}

double poissonCalculate(int simE, const char *variance) {
    double var = std::stof(variance);
    double poisson = (exp(- var)*pow(var, simE))/(factorial(simE));
    return poisson;
}


void poissonReweightingSignal(const char *apMass, const char *variance) // mult is the simulated multiplicity, track is the trk being measured - energy threshold is based off of the trk
{
    gStyle->SetOptStat(0);

    // constants
    vector<int> layerNum = {34,20};
    const char* process = "signal";
    vector<TH1 *> histograms;
    int totalEvents = 0;

    // initializations
    TFile* infile1e = TFile::Open(Form("%s1eAp%sGeV_allruns_trk.root", process, apMass));
    for (int trk = 0; trk <= 5; trk++) {
        for (unsigned int layer = 0; layer < layerNum.size(); layer++) {
            TH1* htemp = (TH1 *)infile1e->Get(Form("TriggerSums%iLayers_%itrk", layerNum[layer], trk));
            totalEvents += htemp->GetEntries();
        }
    }

    for (int trk = 0; trk <= 5; trk++) {
        for (unsigned int layer = 0; layer < layerNum.size(); layer++) {
            TH1* htemp = (TH1 *)infile1e->Get(Form("TriggerSums%iLayers_%itrk", layerNum[layer], trk));
            TH1* hist;
            if (htemp->GetEntries() > 0) {
                hist = htemp->GetCumulative();
            }
            else {
                hist = new TH1F(Form("TriggerSums%iLayers_%itrk_cumulative", layerNum[layer], trk), Form("TriggerSums%iLayers_%itrk_cumulative", layerNum[layer], trk), 100, 0, 20000);
                hist->SetLineWidth(2);
            }
            hist->SetDirectory(0);
            hist->Scale(poissonCalculate(1, variance));
            for (int simE = 2; simE <= 4; simE++) {
                TFile* infiletemp = TFile::Open(Form("%s%ieAp%sGeV_allruns_trk.root", process, simE, apMass));
                TH1* htemp2 = (TH1 *)infiletemp->Get(Form("TriggerSums%iLayers_%itrk", layerNum[layer], trk));
                if (htemp2->GetEntries() > 0) {
                    TH1* hist2 = htemp2->GetCumulative();
                    hist->Add(hist2, poissonCalculate(simE, variance));
                }
            }
            hist->Scale(1./totalEvents);
            histograms.push_back(hist);
        }
    }
    TFile *outfile = new TFile(Form("%sAp%sGeV_allruns_%spoisson.root", process, apMass, variance), "RECREATE");
    for (TH1* h : histograms) {
        h->Write();
    }
    outfile->Close();
}
