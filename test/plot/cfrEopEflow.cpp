/*
 g++ -Wall -o cfrEopEflow `root-config --cflags --glibs` -L $ROOTSYS/lib  cfrEopEflow.cpp
 */


#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"

#include "TProfile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TMath.h"
#include "TCut.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <cmath>
#include <vector>
using namespace std;
bool pos = false; 

void DoPlot( TString file2, TString cut, TString hr, TString output)
{
    TChain* ZeroBias;
    ZeroBias = new TChain("eb");
    ZeroBias -> Add("/afs/cern.ch/work/f/fcetorel/private/work2/prova_slc6/CMSSW_9_4_0/src/PhiSym/EcalCalibAlgos/macros/history_eflow_2017.root" );
    ZeroBias -> SetMarkerStyle(20);
    ZeroBias -> SetMarkerColor(kBlue+2);
    

    TFile* f2 = new TFile(file2);
    cout << f2->GetName() << endl;   
    gStyle->SetOptStat(0);
 TCanvas c;
    c.SetGrid();
    unsigned timemin_nc;
    float  scale_Eop_mean_nc, scale_err_Eop_mean_nc; 
    double mean,emean,norm; 
    TTree *EoPi= (TTree*)f2->Get("ciao");

    TGraphErrors *gr2 = new TGraphErrors(); 

  EoPi->SetBranchAddress("timemin", &timemin_nc); 
  EoPi->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean_nc); 
  EoPi->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean_nc); 
  
  mean = 0; 
  emean = 0; 

  for (int i= 0; i<5; i++) // normalization to the mean of 5 points at the beginning
  {
  EoPi->GetEntry(i);
  mean += scale_Eop_mean_nc*(1./scale_err_Eop_mean_nc)*(1./scale_err_Eop_mean_nc);
  emean += (1./scale_err_Eop_mean_nc)*(1./scale_err_Eop_mean_nc);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  
  //cout << "This is the normalization " << norm << endl;
  //cout << "First point for normalization "<< norm << endl; 
  //cout << "And its error "<< enorm << endl; 

  for (long i = 0; i< EoPi->GetEntries(); i++)
  {
  EoPi->GetEntry(i);
  gr2->SetPoint(i,double(timemin_nc), double(scale_Eop_mean_nc)/norm); 
  gr2->SetPointError(i,0,scale_err_Eop_mean_nc);

  }

   
    gPad->Update();
    TString cut1 = " & n_events > 100e6";
    ZeroBias->Draw("ic_ratio_eflow:avg_time",cut + cut1,"prof");
    TH2F *gr1 = (TH2F*)gPad->GetPrimitive("htemp");
    gr1->SetTitle("; Time(day/month); Relative response ");

    gr1->GetXaxis()->SetTimeDisplay(1);
    gr1->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
    gr1->GetYaxis()->SetRangeUser(0.94,1.025);
    gPad->Update();
    gr2->SetMarkerColor(kRed);
    gr2->SetMarkerStyle(20);
    gr2->Draw("P same");

    TLegend leg; 
    leg.AddEntry(gr1,"IC(t)/IC(0) Eflow","lpe");
    leg.AddEntry(gr2,"E/p","lpe");

    leg.Draw("same");
    c.SaveAs("cfr_"+hr+".png");
    c.SaveAs("cfr_"+hr+".pdf");
    c.SaveAs("cfr_"+hr+".root");
    system("mv cfr_*.root "+output);
    system("mv *.png "+output);
    system("mv *.pdf "+output);
    
}


int main(int argc, char *argv[])
{
pos = (argc!=1 ? 0 : 1);
if (pos) cout << ">>> Doing positive harness " << endl; 
else cout << ">>> Doing negative harness " << endl; 

gStyle->SetOptStat(111);
gStyle->SetOptFit(111);
TString  hr, f_Eop,cut, output; 
output = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/Eflow_Eop"; 

//  module 0
int ietamin=1; 
int ietamax=5; 
int iphimin=1; 
int iphimax=20;

for (int kphi = 0; kphi <18; kphi++ )
{


    output = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/Eflow_Eop"; 
    if (pos)
    {
      hr =("IEta_" +to_string(ietamin)+"_" +to_string(ietamax)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();
      f_Eop="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_noCorr/scaleMonitor_"+hr+".root";
      cut = ("ieta >=" +to_string(ietamin)+" && ieta<=" +to_string(ietamax)+"&& iphi>="+to_string(iphimin+kphi*20)+"&& iphi<="+to_string(iphimax+kphi*20)).c_str();
    }
    else
    {
      hr =("IEta_-" +to_string(ietamax)+"_-" +to_string(ietamin)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();
      f_Eop="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_noCorr/scaleMonitor_"+hr+".root";
      cut=("ieta>=-" +to_string(ietamax)+" && ieta<=-" +to_string(ietamin)+"&& iphi>="+to_string(iphimin+kphi*20)+"&& iphi<="+to_string(iphimax+kphi*20)).c_str();
    }

    DoPlot(f_Eop, cut, hr, output);


  
}

//1,2,3,4 module
ietamin=6;
ietamax=25;
iphimin=1;
iphimax=10;
for (int keta=0; keta<4; keta++ ) //loop over eta
{
  for (int kphi = 0; kphi <36; kphi++ )
  {

    if (pos)
    {
      hr =("IEta_" +to_string(ietamin+keta*20)+"_" +to_string(ietamax+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();
      f_Eop="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_noCorr/scaleMonitor_"+hr+".root";
      cut = ("ieta>=" +to_string(ietamin+keta*20)+" && ieta<=" +to_string(ietamax+keta*20)+"&& iphi>="+to_string(iphimin+kphi*10)+"&& iphi<="+to_string(iphimax+kphi*10)).c_str();

    }
    else
    {
      hr=("IEta_-" +to_string(ietamax+keta*20)+"_-" +to_string(ietamin+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();
      f_Eop="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_noCorr/scaleMonitor_"+hr+".root";
      cut=("ieta>=-" +to_string(ietamax+keta*20)+" & ieta<=-" +to_string(ietamin+keta*20)+" & iphi>="+to_string(iphimin+kphi*10)+" & iphi<="+to_string(iphimax+kphi*10)).c_str();

    }
    DoPlot(f_Eop, cut, hr, output);

  }
}





}
