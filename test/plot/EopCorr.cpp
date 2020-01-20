/*
 g++ -Wall -o EopCorr `root-config --cflags --glibs` -L $ROOTSYS/lib  EopCorr.cpp
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

//void fit_harn(Tstring file; )

int main(int argc, char *argv[])
{

gStyle->SetOptStat(111);
gStyle->SetOptFit(111);
TH1F* histo_slope = new TH1F("histo_slope","harness slope",100,-10.e-9,2.e-9);

  cout << ">>>>>>>> Opening file : ";
  TFile* f = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/all/scaleMonitor_all.root");
  cout << f->GetName() << endl;
  TTree *t= (TTree*)f->Get("ciao");
 unsigned timemin;
  float scale_Eop_mean, scale_err_Eop_mean; 
  double norm; 
  TGraphErrors *gr1 = new TGraphErrors(); 

  t->SetBranchAddress("timemin", &timemin); 
  t->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean); 
  t->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean); 
  

  double mean, emean; 

  for (int i= 0; i<5; i++) // normalization to the w mean of first 5 points
  {
  t->GetEntry(i);
  mean += scale_Eop_mean*(1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  emean += (1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  //cout << "First point for normalization "<< norm << endl; 


  for (long i = 0; i< t->GetEntries(); i++)
  {
  t->GetEntry(i);
  gr1->SetPoint(i,double(timemin), double(scale_Eop_mean)/norm); 
  gr1->SetPointError(i,0,scale_err_Eop_mean);

  }

  TCanvas c; 
  c.cd();
  c.SetGrid(); 
  gr1->SetMarkerColor(kGreen+1);
  gr1->SetMarkerStyle(20); 
  gr1->Draw("ap"); 
  gr1->SetTitle("; Time(day/month); E/p");
  gr1->GetXaxis()->SetTimeDisplay(1);
  gr1->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");

  c.SaveAs("Eop_Barrel.png");
  c.SaveAs("Eop_Barrel.pdf");
  c.SaveAs("Eop_Barrel.root");
  system("mv *.png /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/");
  system("mv *.pdf /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/");
  system("mv *Eop_Barrel.root /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/");
}
