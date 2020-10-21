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
void doEopEflow(TString file1, TString file2,  TString  hr, TString output)
{
    TChain* ZeroBias;
    ZeroBias = new TChain("eb");
    ZeroBias -> Add(file1);
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
    std::string::size_type sz;   // convert string into int
    run_num.push_back(stoi(hr.sustr(4), &sz));   
    run_num.push_back(stoi(title.substr(sz+1)));
    
    TString cut = ("ieta >="+str(ietamin) +" && ieta <="+str(ietamax)+" && iphi >= "+str(iphimin)+" && iphi <="+str(iphimax)).c_str()

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
string method=argv[1];
string fold_EFlow = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/Eflow_Eop/mean/";
string output_EFlow = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/Eflow_Eop/templatefit/";
string output_cfr = "/eos/user/f/fcetorel/www/PhiSym/eflow/closuretest_2018UL_IOV1d/cfr/"+method+"/";

vector<vector<int>> harness = GetHarnessRanges();

  for (unsigned  i = 1; i < harness.size(); i++ )
  {
  cout << harness.at(i).at(0) << endl;
 //doEopEFlow(fold_EFlow ,fold_fit+"fit_"+harness.at(i)+"norm_noCorr.root",  harness.at(i), output_EFlow);
  }
}
