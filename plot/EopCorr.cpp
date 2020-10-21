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
  if (argc < 5 ) 
  {
    cout << ">>>>>>>> Missing some arguments, try : ./EopCorr filename [tecnique = mee , Eop] [method = mean, templatefit] output" << endl;
    return 0;
  }

  TString filename = argv[1];
  TString tec = argv[2];
  TString method = argv[3];
  TString output = argv[4];


  cout << ">>>>>>>> Opening file : ";
  TFile* f = new TFile(filename);
  cout << f->GetName() << endl;
  TTree *t= (TTree*)f->Get("ciao");
 unsigned timemin;
  float scale, scale_unc; 
  double norm; 
  TGraphErrors *gr1 = new TGraphErrors(); 

  t->SetBranchAddress("timemin", &timemin); 
  // t->SetBranchAddress("scale_"+method, &scale); 
  // t->SetBranchAddress("scale_unc_"+method, &scale_unc); 
  t->SetBranchAddress("scale_"+tec+"_"+method, &scale); 
  t->SetBranchAddress("scale_unc_"+tec+"_"+method, &scale_unc); 
  

  double mean, emean; 
  t -> GetEntry(0);
  norm = scale;
  /*
  for (int i= 0; i<5; i++) // normalization to the w mean of first 5 points
  {
  t->GetEntry(i);
  mean += scale*(1./scale_unc)*(1./scale_unc);
  emean += (1./scale_unc)*(1./scale_unc);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  //cout << "First point for normalization "<< norm << endl; 
  */

  for (long i = 0; i< t->GetEntries(); i++)
  {
  t->GetEntry(i);
  gr1->SetPoint(i,double(timemin), double(scale)/norm); 
  gr1->SetPointError(i,0,scale_unc);
  //cout << i << "  " << double(scale)/norm << endl;

  }

  TCanvas c; 
  c.cd();
  c.SetGrid(); 
  gr1->SetMarkerColor(kGreen+1);
  gr1->SetMarkerStyle(20); 
  gr1->Draw("ap"); 
  TString name_scale="E/p";
  if (tec=="mee") name_scale = "Mee";
  gr1->SetTitle("; Time(day/month);"+name_scale);
  gr1->GetXaxis()->SetTimeDisplay(1);
  gr1->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");

  c.SaveAs(tec+"_Barrel_"+method+".png");
  c.SaveAs(tec+"_Barrel_"+method+".pdf");
  c.SaveAs(tec+"_Barrel_"+method+".root");
  system("mv *.png "+output);
  system("mv *.pdf "+output);
  system("mv *_Barrel*.root "+output);
}
