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
  if (argc < 2 ) 
  {
    cout << ">>>>>>>> Missing the method, try : ./EopCorr [method = mean, templatefit] " << endl;
    return 0;
  }

  TString method = argv[1];


  cout << ">>>>>>>> Opening file : ";
  TFile* f = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/Rereco2018/IOV1d/harness_template_barrel/IEta_-85_85_IPhi_1_360/out_file_scalemonitoring.root");
  cout << f->GetName() << endl;
  TTree *t= (TTree*)f->Get("ciao");
 unsigned timemin;
  float scale_Eop, scaleunc_Eop; 
  double norm; 
  TGraphErrors *gr1 = new TGraphErrors(); 

  t->SetBranchAddress("timemin", &timemin); 
  t->SetBranchAddress("scale_Eop_"+method, &scale_Eop); 
  t->SetBranchAddress("scaleunc_Eop_"+method, &scaleunc_Eop); 
  

  double mean, emean; 
  t -> GetEntry(0);
  norm = scale_Eop;
  /*
  for (int i= 0; i<5; i++) // normalization to the w mean of first 5 points
  {
  t->GetEntry(i);
  mean += scale_Eop*(1./scaleunc_Eop)*(1./scaleunc_Eop);
  emean += (1./scaleunc_Eop)*(1./scaleunc_Eop);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  //cout << "First point for normalization "<< norm << endl; 
  */

  for (long i = 0; i< t->GetEntries(); i++)
  {
  t->GetEntry(i);
  gr1->SetPoint(i,double(timemin), double(scale_Eop)/norm); 
  gr1->SetPointError(i,0,scaleunc_Eop);
  cout << i << "  " << double(scale_Eop)/norm << endl;

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

  c.SaveAs("Eop_Barrel_"+method+".png");
  c.SaveAs("Eop_Barrel_"+method+".pdf");
  c.SaveAs("Eop_Barrel_"+method+".root");
  system("mv *.png /eos/user/f/fcetorel/www/PhiSym/eflow/closuretest_2018Rereco/");
  system("mv *.pdf /eos/user/f/fcetorel/www/PhiSym/eflow/closuretest_2018Rereco/");
  system("mv *Eop_Barrel*.root /eos/user/f/fcetorel/www/PhiSym/eflow/closuretest_2018Rereco/");
}
