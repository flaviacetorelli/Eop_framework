/*
 g++ -Wall -o fitter_new `root-config --cflags --glibs` -L $ROOTSYS/lib  fitter_new.cpp
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
bool corr = true;  // bool to decide if run on corr or not, defualt true

void SaveHisto(TH1F* histo, TString name)
{
TCanvas c; 
c.cd(); 
histo->SetLineColor(2); 
histo->Draw("histo");


if (corr)
{ 
c.SaveAs(name+"test.png");
c.SaveAs(name+"test.pdf");
c.SaveAs(name+"test.root");
}

else
{ 
c.SaveAs(name+"_noCorr.png");
c.SaveAs(name+"_noCorr.pdf");
c.SaveAs(name+"_noCorr.root");
}

} 


float GetSlope(TString file, TString hr, TString output)
{

  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111);

  TFile* f = new TFile(file);

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
  cout << "First point for normalization "<< norm << endl; 


  for (long i = 0; i< t->GetEntries(); i++)
  {
  t->GetEntry(i);
  gr1->SetPoint(i,double(timemin), double(scale_Eop_mean)/norm); 
  gr1->SetPointError(i,0,scale_err_Eop_mean);

  }

  TCanvas c; 
  c.cd();
  c.SetGrid(); 
  gr1->SetMarkerColor(2);
  gr1->SetMarkerStyle(21); 
  gr1->Draw("ap"); 


  //gr1->GetYaxis()->SetRangeUser(0.94,1.06);  
  TF1 *fittino = new TF1 ("fittino", "pol1"); 
  c.Update(); 
  gr1->Fit("fittino","q");
  cout << "Slope of the function for harness "<< hr << ": "<<fittino->GetParameter(1) << endl; 
  
  if( corr )
  {
    c.SaveAs("fit_"+hr+"norm_test.png");
    c.SaveAs("fit_"+hr+"norm_test.pdf");
    c.SaveAs("fit_"+hr+"norm_test.root");
  }

  else
  {
    c.SaveAs("fit_"+hr+"norm_noCorr.png");
    c.SaveAs("fit_"+hr+"norm_noCorr.pdf");
    c.SaveAs("fit_"+hr+"norm_noCorr.root");
  }

  system("mv *.png "+output);
  system("mv *.pdf "+output);
  system("mv *fit_*.root "+output);
  return fittino->GetParameter(1); 

}




int main(int argc, char *argv[])
{

corr = (argc!=1 ? 0 : 1);
if (corr) cout << ">>> Doing corrected ones " << endl; 
else cout << ">>> Doing not corrected ones " << endl; 

gStyle->SetOptStat(111);
gStyle->SetOptFit(111);
TH1F* histo_slope = new TH1F("histo_slope","harness slope",100,-10.e-9,2.e-9); //histo of slope, inclusive and divided by modules
TH1F* histo_slope_mod01 = new TH1F("histo_slope_mod01","harness slope",100,-10.e-9,2.e-9);
//TH1F* histo_slope_mod23 = new TH1F("histo_slope_mod23","harness slope",100,-10.e-9,2.e-9);
TH1F* histo_slope_mod2 = new TH1F("histo_slope_mod2","harness slope",100,-10.e-9,2.e-9);
TH1F* histo_slope_mod3 = new TH1F("histo_slope_mod3","harness slope",100,-10.e-9,2.e-9);
TH1F* histo_slope_mod4 = new TH1F("histo_slope_mod4","harness slope",100,-10.e-9,2.e-9);

//  module 0
int ietamin=1; 
int ietamax=5; 
int iphimin=1; 
int iphimax=20;
TString output="/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/fit/wEopCorr1"; 

for (int kphi = 0; kphi <18; kphi++ )
{
  TString hr_pos =("IEta_" +to_string(ietamin)+"_" +to_string(ietamax)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();
  TString hr_neg =("IEta_-" +to_string(ietamax)+"_-" +to_string(ietamin)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();

  if (corr)
  {
    TString f_pos_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_test1/scaleMonitor_"+hr_pos+".root";
    TString f_neg_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_test1/scaleMonitor_"+hr_neg+".root";
    double slope_p = GetSlope(f_pos_corr, hr_pos, output);  
    double slope_n = GetSlope(f_neg_corr, hr_neg, output);

    histo_slope -> Fill(slope_n);  
    histo_slope -> Fill(slope_p);
    histo_slope_mod01 -> Fill(slope_n);  
    histo_slope_mod01 -> Fill(slope_p);    
  } 

  else 
  {
    output = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/fit/"; 
    TString f_pos_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_noCorr/scaleMonitor_"+hr_pos+".root";
    TString f_neg_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_noCorr/scaleMonitor_"+hr_neg+".root";
    double slope_p = GetSlope(f_pos_noCorr, hr_pos, output);
    double slope_n = GetSlope(f_neg_noCorr, hr_neg, output);
    histo_slope -> Fill(slope_n);  
    histo_slope -> Fill(slope_p);
    histo_slope_mod01 -> Fill(slope_n);  
    histo_slope_mod01 -> Fill(slope_p);
  }


}

//1,2,3,4 modules
ietamin=6;
ietamax=25;
iphimin=1;
iphimax=10;
for (int keta=0; keta<4; keta++ ) //loop over eta
{
  for (int kphi = 0; kphi <36; kphi++ )
  {
    TString hr_pos =("IEta_" +to_string(ietamin+keta*20)+"_" +to_string(ietamax+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();
    TString hr_neg =("IEta_-" +to_string(ietamax+keta*20)+"_-" +to_string(ietamin+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();
  if (corr)
  {
    TString f_pos_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_test1/scaleMonitor_"+hr_pos+".root";
    TString f_neg_corr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_test1/scaleMonitor_"+hr_neg+".root";
    double slope_p=GetSlope(f_pos_corr, hr_pos, output);
    double slope_n=GetSlope(f_neg_corr, hr_neg, output);
    histo_slope -> Fill(slope_n);  
    histo_slope -> Fill(slope_p);
    if (keta==0) 
    {
      histo_slope_mod01 -> Fill(slope_n);  
      histo_slope_mod01 -> Fill(slope_p);  
    }
    if (keta==1 ) 
    {
      histo_slope_mod2 -> Fill(slope_n);  
      histo_slope_mod2 -> Fill(slope_p);  
    }
    if ( keta==2) 
    {
      histo_slope_mod3 -> Fill(slope_n);  
      histo_slope_mod3 -> Fill(slope_p);  
    }
    if (keta==3) 
    {
      histo_slope_mod4 -> Fill(slope_n);  
      histo_slope_mod4 -> Fill(slope_p); 
    }
  } 

  else 
  {
    output = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/fit/";
    TString f_pos_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_noCorr/scaleMonitor_"+hr_pos+".root";
    TString f_neg_noCorr="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_negative_noCorr/scaleMonitor_"+hr_neg+".root";

    double slope_p=GetSlope(f_pos_noCorr, hr_pos, output);
    double slope_n=GetSlope(f_neg_noCorr, hr_neg, output);
    histo_slope -> Fill(slope_n);  
    histo_slope -> Fill(slope_p);
    if (keta==0) 
    {
      histo_slope_mod01 -> Fill(slope_n);  
      histo_slope_mod01 -> Fill(slope_p);  
    }
    if (keta==1 ) 
    {
      histo_slope_mod2 -> Fill(slope_n);  
      histo_slope_mod2 -> Fill(slope_p);  
    }
    if ( keta==2) 
    {
      histo_slope_mod3 -> Fill(slope_n);  
      histo_slope_mod3 -> Fill(slope_p);  
    }
    if (keta==3) 
    {
      histo_slope_mod4 -> Fill(slope_n);  
      histo_slope_mod4 -> Fill(slope_p); 
    }

    
  }

  }
}

SaveHisto(histo_slope, "histo_slope_norm1");
SaveHisto(histo_slope_mod01, "histo_slope_mod01_norm1");
SaveHisto(histo_slope_mod2, "histo_slope_mod2_norm1");
SaveHisto(histo_slope_mod3, "histo_slope_mod3_norm1");
SaveHisto(histo_slope_mod4, "histo_slope_mod4_norm1");


system("mv histo_slope*.root /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
system("mv histo_slope*.png /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
system("mv histo_slope*.pdf /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");


}
