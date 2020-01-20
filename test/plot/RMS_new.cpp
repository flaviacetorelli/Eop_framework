/*
 g++ -Wall -o RMS `root-config --cflags --glibs` -L $ROOTSYS/lib  RMS.cpp
./RMS_new [pathtonegative] [pathtopositive] [label] (label=test,--,noCorr)
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
bool corr = false; 
//void fit_harn(Tstring file; )


vector<double> doMean(TString file)
{

  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111);

  TFile* f = new TFile(file);
  cout << f->GetName() << endl;

  TTree *t1= (TTree*)f->Get("ciao");
  unsigned timemin;
  float scale_Eop_mean, scale_err_Eop_mean; 
  double norm; 

  t1->SetBranchAddress("timemin", &timemin); 
  t1->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean); 
  t1->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean); 
  double mean, emean; 

  for (int i= 0; i<5; i++) // normalization to the w mean of the first 5 points
  {
  t1->GetEntry(i);
  mean += scale_Eop_mean*(1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  emean += (1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
  //cout << "This is mean" << mean << endl; 
  }
  norm = mean/emean;
  
  //cout << "Mean for normalization "<< norm << endl; 
  //cout << "And its error "<< enorm << endl; 
  vector<double> a; 
  for (long i = 0; i< t1->GetEntries(); i++)
  {
  t1->GetEntry(i);
  a.push_back(double(scale_Eop_mean)/norm); 

  }
 

  return a; 
}

void doRMSPlots(TH1F** histo, TString title, TString output)
{
 TFile* f = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/harness_positive_noCorr/scaleMonitor_IEta_1_5_IPhi_1_20.root");
  //cout << f->GetName() << endl;

  TTree *t= (TTree*)f->Get("ciao");
  unsigned timemin;
  t->SetBranchAddress("timemin", &timemin); 
   TCanvas c; 
   c.cd(); 
   TF1 *gs = new TF1 ("gs","gaus");
    TGraphErrors* gr = new TGraphErrors();
    for (unsigned ibin=0; ibin<100; ibin++)
    {
      histo[ibin]->Fit("gs","q");  
      histo[ibin]->Draw();
      c.SaveAs(title+("_histo_"+to_string(ibin)+".png").c_str());
      c.SaveAs(title+("_histo_"+to_string(ibin)+".pdf").c_str());
      c.SaveAs(title+("_histo_"+to_string(ibin)+".root").c_str());
      system("mv RMS*.root " +output+"/histo");
      system("mv *.png " +output+"/histo");
      system("mv *.pdf " +output+"/histo");
      t->GetEntry(ibin);
      //gr->SetPoint(ibin,timemin,histo[ibin]->GetRMS());
      gr->SetPoint(ibin,timemin,gs->GetParameter(2));

      //gr->SetPointError(ibin,0,histo[ibin]->GetRMSError());
      gr->SetPointError(ibin,0,gs->GetParError(2));

    }

   c.SetGrid(); 
   gr->SetName("gr"); 
   gr->SetMarkerStyle(21);
   gr->SetMarkerColor(2);
   gr->Draw("ap");
   gr->GetYaxis()->SetRangeUser(0,0.01);
   gr->SetTitle("; Time(day/month); RMS");
   gr->GetXaxis()->SetTimeDisplay(1);
   gr->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
   c.Update(); 

   c.SaveAs(title+".png");
   c.SaveAs(title+".pdf");
   c.SaveAs(title+".root");
   
   
   
system("mv RMS*.root "+output);
system("mv *.png "+output);
system("mv *.pdf "+output);

}


int main(int argc, char *argv[])
{
if (argc<3)cout<< "Missing some informations: ./RMS [path_to_hr-] [path_to_hr-] [label]" << endl;
cout << ">>> Doing these samples " << endl; 
cout << argv[1] << endl; 
cout << argv[2] << endl; 


//
string fold = "/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/";
string neg = argv[1];
string pos = argv[2];
string label;
argc>3 ?  label= argv[3] : label="";
TString output="/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS";

gStyle->SetOptStat(111);
gStyle->SetOptFit(111);
TH1F* histo[100];
TH1F* histo_01[100];
TH1F* histo_2[100];
TH1F* histo_3[100];
TH1F* histo_4[100];

  for (int ibin=0; ibin<100; ibin++)
  {
    histo[ibin] = new TH1F(("histo_"+ std::to_string(ibin)).c_str(), "; Eop_mean; Counts", 50, 0.94, 1.04 );
    histo_01[ibin] = new TH1F(("histo_01_"+ std::to_string(ibin)).c_str(), "; Eop_mean; Counts", 50, 0.94, 1.04 );
    histo_2[ibin] = new TH1F(("histo_2_"+ std::to_string(ibin)).c_str(), "; Eop_mean; Counts", 50, 0.94, 1.04 );
    histo_3[ibin] = new TH1F(("histo_3_"+ std::to_string(ibin)).c_str(), "; Eop_mean; Counts", 50, 0.94, 1.04 );
    histo_4[ibin] = new TH1F(("histo_4_"+ std::to_string(ibin)).c_str(), "; Eop_mean; Counts", 50, 0.94, 1.04);
  }

//  module 0
int ietamin=1; 
int ietamax=5; 
int iphimin=1; 
int iphimax=20;

for (int kphi = 0; kphi <18; kphi++ )
{
  TString hr_pos =("IEta_" +to_string(ietamin)+"_" +to_string(ietamax)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();
  TString hr_neg =("IEta_-" +to_string(ietamax)+"_-" +to_string(ietamin)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str();


  TString f_pos=fold+pos+"/scaleMonitor_"+hr_pos+".root";
  TString f_neg=fold+neg+"/scaleMonitor_"+hr_neg+".root";
  
  
  vector<double>  mean_p = doMean(f_pos);
  vector<double>  mean_n = doMean(f_neg);
  for (unsigned ibin=0; ibin<mean_n.size(); ibin++) // riempie i vari histo divisi per bin temporali
  {
    histo[ibin]->Fill(mean_n.at(ibin));
    histo[ibin]->Fill(mean_p.at(ibin));
    histo_01[ibin]->Fill(mean_n.at(ibin)); 
    histo_01[ibin]->Fill(mean_p.at(ibin));
  }
  

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
    TString hr_pos =("IEta_" +to_string(ietamin+keta*20)+"_" +to_string(ietamax+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();
    TString hr_neg =("IEta_-" +to_string(ietamax+keta*20)+"_-" +to_string(ietamin+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str();

  TString f_pos=fold+pos+"/scaleMonitor_"+hr_pos+".root";
  TString f_neg=fold+neg+"/scaleMonitor_"+hr_neg+".root";
  
 
    vector<double>  mean_p = doMean(f_pos);
    vector<double>  mean_n = doMean(f_neg);
    for (unsigned ibin=0; ibin<mean_n.size(); ibin++)
    {
      histo[ibin]->Fill(mean_n.at(ibin));
      histo[ibin]->Fill(mean_p.at(ibin));
      if (keta==0 )
      {
        histo_01[ibin]->Fill(mean_n.at(ibin)); 
        histo_01[ibin]->Fill(mean_p.at(ibin));
      }
      if (keta==1) 
      {
        histo_2[ibin]->Fill(mean_n.at(ibin)); 
        histo_2[ibin]->Fill(mean_p.at(ibin));
      }
      if (keta==2) 
      {
        histo_3[ibin]->Fill(mean_n.at(ibin)); 
        histo_3[ibin]->Fill(mean_p.at(ibin));
      }
      if (keta==3) 
      {
      histo_4[ibin]->Fill(mean_n.at(ibin));
      histo_4[ibin]->Fill(mean_p.at(ibin));

      }


    }
  }  
}

  doRMSPlots(histo, "RMS"+label,output); 
  doRMSPlots(histo_01, "RMS_mod01"+label,output); 
  doRMSPlots(histo_2, "RMS_mod2"+label,output); 
  doRMSPlots(histo_3, "RMS_mod3"+label,output); 
  doRMSPlots(histo_4, "RMS_mod4"+label,output); 
  //cout << f->GetName() << endl;


}
