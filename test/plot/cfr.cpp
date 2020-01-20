/*
 g++ -Wall -o cfr `root-config --cflags --glibs` -L $ROOTSYS/lib  cfr.cpp
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
//void fit_harn(Tstring file; )


void DoPlot(TString filecorr, TString filenoCorr, TString hr, TString output)
{

  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111);

  TFile* f_corr = new TFile(filecorr);
  cout << f_corr->GetName() << endl;

  TTree *t1= (TTree*)f_corr->Get("ciao");
  unsigned timemin, timemin_nc;
  float scale_Eop_mean, scale_err_Eop_mean, scale_Eop_mean_nc, scale_err_Eop_mean_nc; 
  double norm; 
  TGraphErrors *gr1 = new TGraphErrors(); 

  t1->SetBranchAddress("timemin", &timemin); 
  t1->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean); 
  t1->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean); 
  double mean, emean; 

  for (int i= 0; i<5; i++) // normalization to the mean of 3 points in the middle (stable period)
  {
  t1->GetEntry(i);
  mean += scale_Eop_mean*(1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  emean += (1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  
  //cout << "Mean for normalization "<< norm << endl; 
  //cout << "And its error "<< enorm << endl; 

  for (long i = 0; i< t1->GetEntries(); i++)
  {
  t1->GetEntry(i);
  gr1->SetPoint(i,double(timemin), double(scale_Eop_mean)/norm); 
  gr1->SetPointError(i,0,scale_err_Eop_mean);

  }

  TFile* f2 = new TFile(filenoCorr);
  cout << f2->GetName() << endl;

  TTree *t2= (TTree*)f2->Get("ciao");

  TGraphErrors *gr2 = new TGraphErrors(); 

  t2->SetBranchAddress("timemin", &timemin_nc); 
  t2->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean_nc); 
  t2->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean_nc); 
  
  mean = 0; 
  emean = 0; 

  for (int i= 0; i<5; i++) // normalization to the mean of 3 points in the middle (stable period)
  {
  t2->GetEntry(i);
  mean += scale_Eop_mean_nc*(1./scale_err_Eop_mean_nc)*(1./scale_err_Eop_mean_nc);
  emean += (1./scale_err_Eop_mean_nc)*(1./scale_err_Eop_mean_nc);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  
  //cout << "This is the normalization " << norm << endl;
  //cout << "First point for normalization "<< norm << endl; 
  //cout << "And its error "<< enorm << endl; 

  for (long i = 0; i< t2->GetEntries(); i++)
  {
  t2->GetEntry(i);
  gr2->SetPoint(i,double(timemin_nc), double(scale_Eop_mean_nc)/norm); 
  gr2->SetPointError(i,0,scale_err_Eop_mean_nc);

  }

    TCanvas c; 
    c.cd(); 
    c.SetGrid(); 
    gr1->SetMarkerColor(kRed+1);
    gr2->SetMarkerColor(kBlue+1);
    gr1->SetMarkerStyle(21);
    gr2->SetMarkerStyle(21);


    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gr1);
    mg->Add(gr2);
    mg->Draw("ap");

    mg->SetTitle("; Time(day/month); E/p ");
    mg->GetXaxis()->SetTimeDisplay(1);
    mg->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
    mg->GetYaxis()->SetRangeUser(0.94,1.04);
    TLegend leg; 
    leg.AddEntry(gr1,"corrected","lpe");
    leg.AddEntry(gr2,"not corrected","lpe");

    leg.Draw("same");
    c.SaveAs("cfr_"+hr+".png");
    c.SaveAs("cfr_"+hr+".pdf");
    c.SaveAs("cfr_"+hr+".root");
system("mv cfr_*.root "+output);
system("mv *.png "+output);
system("mv *.pdf "+output);
}


void DoPlot(TString filecorr, TString filenoCorr, TString filetest, TString hr, TString output)
{

  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111);

  TFile* f_corr = new TFile(filecorr);
  cout << f_corr->GetName() << endl;

  TTree *t1= (TTree*)f_corr->Get("ciao");
  unsigned timemin, timemin_nc, timemin_test;
  float scale_Eop_mean, scale_err_Eop_mean, scale_Eop_mean_nc, scale_err_Eop_mean_nc, scale_Eop_mean_test, scale_err_Eop_mean_test; 
  double norm; 
  TGraphErrors *gr1 = new TGraphErrors(); 

  t1->SetBranchAddress("timemin", &timemin); 
  t1->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean); 
  t1->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean); 
  double mean, emean; 

  for (int i= 0; i<5; i++) // normalization to the mean of 3 points in the middle (stable period)
  {
  t1->GetEntry(i);
  mean += scale_Eop_mean*(1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  emean += (1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  
  //cout << "Mean for normalization "<< norm << endl; 
  //cout << "And its error "<< enorm << endl; 

  for (long i = 0; i< t1->GetEntries(); i++)
  {
  t1->GetEntry(i);
  gr1->SetPoint(i,double(timemin), double(scale_Eop_mean)/norm); 
  gr1->SetPointError(i,0,scale_err_Eop_mean);

  }

  TFile* f2 = new TFile(filenoCorr);
  cout << f2->GetName() << endl;

  TTree *t2= (TTree*)f2->Get("ciao");

  TGraphErrors *gr2 = new TGraphErrors(); 

  t2->SetBranchAddress("timemin", &timemin_nc); 
  t2->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean_nc); 
  t2->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean_nc); 
  
  mean = 0; 
  emean = 0; 

  for (int i= 0; i<5; i++) // normalization to the mean of 3 points in the middle (stable period)
  {
  t2->GetEntry(i);
  mean += scale_Eop_mean_nc*(1./scale_err_Eop_mean_nc)*(1./scale_err_Eop_mean_nc);
  emean += (1./scale_err_Eop_mean_nc)*(1./scale_err_Eop_mean_nc);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  
  //cout << "This is the normalization " << norm << endl;
  //cout << "First point for normalization "<< norm << endl; 
  //cout << "And its error "<< enorm << endl; 

  for (long i = 0; i< t2->GetEntries(); i++)
  {
  t2->GetEntry(i);
  gr2->SetPoint(i,double(timemin_nc), double(scale_Eop_mean_nc)/norm); 
  gr2->SetPointError(i,0,scale_err_Eop_mean_nc);

  }
  TFile* f3 = new TFile(filetest);
  cout << f3->GetName() << endl;

  TTree *t3= (TTree*)f3->Get("ciao");

  TGraphErrors *gr3 = new TGraphErrors(); 

  t3->SetBranchAddress("timemin", &timemin_test); 
  t3->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean_test); 
  t3->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean_test); 
  
  mean = 0; 
  emean = 0; 

  for (int i= 0; i<5; i++) // normalization to the mean of 3 points in the middle (stable period)
  {
  t3->GetEntry(i);
  mean += scale_Eop_mean_test*(1./scale_err_Eop_mean_test)*(1./scale_err_Eop_mean_test);
  emean += (1./scale_err_Eop_mean_test)*(1./scale_err_Eop_mean_test);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  
  //cout << "This is the normalization " << norm << endl;
  //cout << "First point for normalization "<< norm << endl; 
  //cout << "And its error "<< enorm << endl; 

  for (long i = 0; i< t3->GetEntries(); i++)
  {
  t3->GetEntry(i);
  gr3->SetPoint(i,double(timemin_test), double(scale_Eop_mean_test)/norm); 
  gr3->SetPointError(i,0,scale_err_Eop_mean_test);

  }
    TCanvas c; 
    c.cd(); 
    c.SetGrid(); 
    gr1->SetMarkerColor(kRed+1);
    gr2->SetMarkerColor(kBlue+1);
    gr3->SetMarkerColor(kGreen+2);
    gr1->SetMarkerStyle(21);
    gr2->SetMarkerStyle(21);
    gr3->SetMarkerStyle(21);


    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gr1);
    mg->Add(gr2);
    mg->Add(gr3);
    mg->Draw("ap");
    mg->SetTitle("; Time(day/month); E/p ");
    mg->GetXaxis()->SetTimeDisplay(1);
    mg->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
    mg->GetYaxis()->SetRangeUser(0.94,1.04);

    TLegend leg; 

    leg.AddEntry(gr2,"not corrected","lpe");
    leg.AddEntry(gr1,"corrected w Eflow","lpe");
    leg.AddEntry(gr3,"corrected w Eflow + Eop","lpe");

    leg.Draw("same");
    c.SaveAs("cfr_"+hr+".png");
    c.SaveAs("cfr_"+hr+".pdf");
    c.SaveAs("cfr_"+hr+".root");


system("mv cfr_*.root "+output+"wEopCorr");
system("mv *.png "+output+"wEopCorr");
system("mv *.pdf "+output+"wEopCorr");
}


int main(int argc, char *argv[])
{
pos = (argc!=1 ? 0 : 1);
if (pos) cout << ">>> Doing positive harness " << endl; 
else cout << ">>> Doing negative harness " << endl; 

gStyle->SetOptStat(111);
gStyle->SetOptFit(111);

TString output="/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/cfr/";
//  module 0
int ietamin=1; 
int ietamax=5; 
int iphimin=1; 
int iphimax=20;

for (int kphi = 0; kphi <18; kphi++ )
{

    TString hr, f_corr, f_noCorr, f_test; 
    if (pos)
    {
      hr =("IEta_" +to_string(ietamin)+"_" +to_string(ietamax)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();
      f_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive/scaleMonitor_"+hr+".root";
      f_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_noCorr/scaleMonitor_"+hr+".root";
      f_test="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_test/scaleMonitor_"+hr+".root";
    }
    else
    {
      hr =("IEta_-" +to_string(ietamax)+"_-" +to_string(ietamin)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();
      f_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative/scaleMonitor_"+hr+".root"; 
      f_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_noCorr/scaleMonitor_"+hr+".root";
      f_test="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_test/scaleMonitor_"+hr+".root"; 
    }
    DoPlot(f_corr, f_noCorr, hr,output);
    //DoPlot(f_corr, f_noCorr, f_test, hr);

  
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
    TString hr, f_corr, f_noCorr, f_test; 
    if (pos)
    {
      hr =("IEta_" +to_string(ietamin+keta*20)+"_" +to_string(ietamax+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();
      f_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive/scaleMonitor_"+hr+".root";
      f_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_noCorr/scaleMonitor_"+hr+".root";
      f_test="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_test/scaleMonitor_"+hr+".root";
    }
    else
    {
      hr=("IEta_-" +to_string(ietamax+keta*20)+"_-" +to_string(ietamin+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();
      f_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative/scaleMonitor_"+hr+".root"; 
      f_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_noCorr/scaleMonitor_"+hr+".root";
      f_test="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_test/scaleMonitor_"+hr+".root"; 
    }
    DoPlot(f_corr, f_noCorr, hr,output);
    //DoPlot(f_corr, f_noCorr,f_test, hr);

  }
}





}
