/*
 g++ -Wall -o fitter `root-config --cflags --glibs` -L $ROOTSYS/lib  fitter.cpp
 */

#include "harness.h"
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

void SaveHisto(TH1F* histo, TString label, TString name)
{
  TCanvas c; 
  c.cd(); 
  histo->SetLineColor(2); 
  histo->Draw("histo");

  c.SaveAs(name+"_"+label+".png");
  c.SaveAs(name+"_"+label+".pdf");
  c.SaveAs(name+"_"+label+".root");
} 


float GetSlope(TString file, TString hr, TString method, TString label, TString output)
{

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  //TFile* f = new TFile(file);
  TChain *t = new TChain("ciao");
  //cout << f->GetName() << endl;
  cout << "Adding this files to the chain : " << file << endl;
  t->Add(file);
  //TTree *t= (TTree*)f->Get("ciao");
  unsigned timemin;
  float scale_Eop, scaleunc_Eop; 
  double norm; 
  TGraphErrors *gr1 = new TGraphErrors(); 
  gr1->SetName("gr"); 
  t->SetBranchAddress("timemin", &timemin); 
  t->SetBranchAddress("scale_Eop_"+method, &scale_Eop); 
  t->SetBranchAddress("scale_unc_Eop_"+method, &scaleunc_Eop); 
  

  double mean, emean; 

  for (int i= 0; i<5; i++) // normalization to the w mean of first 5 points
  {
  t->GetEntry(i);
  mean += scale_Eop*(1./scaleunc_Eop)*(1./scaleunc_Eop);
  emean += (1./scaleunc_Eop)*(1./scaleunc_Eop);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  //cout << "First point for normalization "<< norm << endl; 


  for (long i = 0; i< t->GetEntries(); i++)
  {
  t->GetEntry(i);
  
  if (scale_Eop > 0 )
  {
    gr1->SetPoint(i,double(timemin), double(scale_Eop)/norm); 
    gr1->SetPointError(i,0,scaleunc_Eop);
    //cout << "This is E/p = " << scale_Eop << endl;
  }

  }

  TCanvas c; 
  c.cd();
  c.SetGrid(); 
  gr1->SetMarkerColor(2);
  gr1->SetMarkerStyle(21); 
  gr1->SetTitle("; Time(day/month); E/p");
  gr1->Draw("ap"); 
  gr1->GetXaxis()->SetTimeDisplay(1);
  gr1->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");


  //gr1->GetYaxis()->SetRangeUser(0.94,1.06);  
  TF1 *fittino = new TF1 ("fittino", "pol1"); 
  c.Update(); 
  gr1->Fit("fittino","q");
  //cout << "Slope of the function for harness "<< hr << ": "<<fittino->GetParameter(1) << endl; 
  

  c.SaveAs("fit_"+hr+"norm_"+label+".png");
  c.SaveAs("fit_"+hr+"norm_"+label+".pdf");
  c.SaveAs("fit_"+hr+"norm_"+label+".root");

  system ("mkdir -p "+output);
 
  system("mv *.png "+output);
  system("mv *.pdf "+output);
  system("mv *fit_*.root "+output);

  return fittino->GetParameter(1); 

}


int main(int argc, char *argv[])
{

 if (argc<5)
 {  
   cout<< "Missing some informations: \
   ./fitter [fold_ntuple] [label=noCorr,EFlowCorr,EopCorr, ZeeCorr] [method=mean, median, templatefit] [output]" << endl;
   return -1;  
  }
  cout << ">>> Doing these samples " << endl; 
  cout << argv[1] << endl; 

  //Reading command lines options
  string ntuple = argv[1];
  string label = argv[2];
  string method = argv[3];
  string output_foldname = argv[4];


  gStyle->SetOptStat(111);
  gStyle->SetOptFit(111);
  TH1F* histo_slope = new TH1F("histo_slope","harness slope",100,-10.e-9,2.e-9); //histo of slope, inclusive and divided by modules
  TH1F* histo_slope_mod01 = new TH1F("histo_slope_mod01","harness slope",100,-10.e-9,2.e-9);
  TH1F* histo_slope_mod2 = new TH1F("histo_slope_mod2","harness slope",100,-10.e-9,2.e-9);
  TH1F* histo_slope_mod3 = new TH1F("histo_slope_mod3","harness slope",100,-10.e-9,2.e-9);
  TH1F* histo_slope_mod4 = new TH1F("histo_slope_mod4","harness slope",100,-10.e-9,2.e-9);


  TString output="/eos/user/f/fcetorel/www/PhiSym/eflow/"+output_foldname+"/fit/"+method+"/"; 
  TString output_slope = "/eos/user/f/fcetorel/www/PhiSym/eflow/"+output_foldname+"/histo_slope/"+method+"/";
  vector<string> harness = GetHarnessRanges();
 
  //create the outputs directory
  
  system ("mkdir -p "+output);
  system ("mkdir -p "+output_slope);
 
  for (unsigned  i = 0; i < harness.size(); i++ ) //fit all harnesses and get their slope
  {
    double slope = 0.;


    TString fold="/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/"+ntuple+"/";
    TString file=fold + harness.at(i)+"/out_file_*_scalemonitoring.root";
    slope = GetSlope(file, harness.at(i), method, label, output);  

    histo_slope -> Fill(slope);  

     int etamin = stoi((harness.at(i)).substr(5),nullptr);
    //cout << "This is " << etamin << endl; 
    if (etamin == -5 || etamin == 1 || etamin == 6 || etamin== -25 ){ histo_slope_mod01 -> Fill(slope); cout << "Filling 1" << endl;}
    else if (etamin == -45 || etamin== 26 ) {histo_slope_mod2 -> Fill(slope);  cout << "Filling 2" << endl;}
    else if (etamin == -65 || etamin== 46 ) {histo_slope_mod3 -> Fill(slope);  cout << "Filling 3" << endl;}
    else if (etamin == -85 || etamin== 66 ) {histo_slope_mod4 -> Fill(slope);  cout << "Filling 4" << endl;}
   }

  
  SaveHisto(histo_slope, label, output_slope+"/inclusive_norm");
  SaveHisto(histo_slope_mod01, label,  output_slope+"/mod01_norm");
  SaveHisto(histo_slope_mod2, label,  output_slope+"/mod2_norm");
  SaveHisto(histo_slope_mod3, label, output_slope+"/mod3_norm");
  SaveHisto(histo_slope_mod4, label, output_slope+"/mod4_norm");

}

