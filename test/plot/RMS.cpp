/*
 g++ -Wall -o RMS `root-config --cflags --glibs` -L $ROOTSYS/lib  RMS.cpp
./RMS [folder_ntuple] [label] [method] (label=noCorr, EFlowCorr, EopCorr) (method=mean, median, templatefit)
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
bool corr = false; 



vector<double> doMean(TString file, TString method)
{
 gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111);


  TChain *t = new TChain("ciao");
  cout << "Adding this files to the chain : " << file << endl;
  t->Add(file);

  
  unsigned timemin;
  float scale_Eop, scaleunc_Eop; 
  double norm; 

  t->SetBranchAddress("timemin", &timemin); 
  t->SetBranchAddress("scale_Eop_"+method, &scale_Eop); 
  t->SetBranchAddress("scaleunc_Eop_"+method, &scaleunc_Eop); 


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

  vector<double> a; 
  for (long i = 0; i< t->GetEntries(); i++)
  {
  t->GetEntry(i);
  a.push_back(double(scale_Eop)/norm); 

  }
 

  return a; 
}

void doRMSPlots(TH1F** histo, TString title ,TString output)
{
 TFile* f = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/barrel_template_EflowCorr/IEta_-85_85_IPhi_1_360/out_file_scalemonitoring.root");
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
if (argc<4)cout<< "Missing some informations: ./RMS [fold_ntuple] [label=noCorr,EFlowCorr,EopCorr] [method=mean, median, templatefit]" << endl;
cout << ">>> Doing these samples " << endl; 
cout << argv[1] << endl; 



//
string fold = "/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/";
string ntuple = argv[1];
string label = argv[2];
string method = argv[3];
vector<string> harness = GetHarnessRanges();
TString output="/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS/"+method+"/";

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


  for (unsigned  i = 0; i < harness.size(); i++ )
  {

    TString file=fold+ntuple+"/"+harness.at(i)+"/out_file_*_scalemonitoring.root";
    int etamin = stoi((harness.at(i)).substr(5),nullptr);
    //cout << "This is " << etamin << endl;   
  
    vector<double>  mean = doMean(file, method);

    for (unsigned ibin=0; ibin<mean.size(); ibin++) // riempie i vari histo divisi per bin temporali
    {
      histo[ibin]->Fill(mean.at(ibin));


      if (etamin == -5 || etamin == 1 || etamin == 6 || etamin== -25 )
      {
        histo_01[ibin]->Fill(mean.at(ibin)); 
      }
      else if (etamin == -45 || etamin== 26 ) 
      {
        histo_2[ibin]->Fill(mean.at(ibin));
      }
      else if (etamin == -65 || etamin== 46 ) 
      {
        histo_3[ibin]->Fill(mean.at(ibin)); 
      }
      else if (etamin == -85 || etamin== 66)
      {
      histo_4[ibin]->Fill(mean.at(ibin));
      }
   }
  
 
  }

  doRMSPlots(histo, "RMS_"+label, output); 
  doRMSPlots(histo_01, "RMS_mod01_"+label, output); 
  doRMSPlots(histo_2, "RMS_mod2_"+label,output); 
  doRMSPlots(histo_3, "RMS_mod3_"+label,output); 
  doRMSPlots(histo_4, "RMS_mod4_"+label,  output); 
  //cout << f->GetName() << endl;


}
