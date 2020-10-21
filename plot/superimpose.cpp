/*
 g++ -Wall -o superimpose `root-config --cflags --glibs` -L $ROOTSYS/lib  superimpose.cpp
 */

//#include "graph_utils.h"
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
void doEopEFlow( TString file1, TString file2, TString nameCanvas,  TString nameOBJ, TString hr, TString output)
{

    
  TFile *f1 = new TFile(file1);
  if (!(f1->IsOpen())) return; 
  TCanvas *c1 = (TCanvas*)f1->Get(nameCanvas);
  TGraph *gr1=(TGraph*)c1->GetListOfPrimitives()->FindObject("htemp");
  gr1->SetTitle(";;");

  TFile *f2 = new TFile(file2);
  TCanvas *c2 = (TCanvas*)f2->Get(nameCanvas);
  TGraphErrors *gr2=(TGraphErrors*)c2->GetListOfPrimitives()->FindObject(nameOBJ);
  TF1 *fittinos= (TF1*)gr2->GetFunction("fittino");
  fittinos->SetLineWidth(0);


  TCanvas c; 
  c.cd(); 
  c.SetGrid(); 
  TMultiGraph *mg = new TMultiGraph();


  gr1->SetMarkerColor(kBlue+1);
  gr1->SetMarkerStyle(20);
  gr2->SetMarkerColor(kRed+1);
  gr2->SetMarkerStyle(21);
  mg->Add(gr1);
  mg->Add(gr2);



  mg->Draw("ap");
  mg->SetTitle("; Time(day/month); Relative Response");
  mg->GetYaxis()->SetRangeUser(0.94,1.025);
  mg->GetXaxis()->SetTimeDisplay(1);
  mg->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");


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


void doSuperTGraph(string fold, vector<string> file, TString nameCanvas,  TString nameOBJ, TString yaxis, vector<double> Yrange,   TString title, vector<string> label, bool doRMS)
{
  vector<TGraphErrors*> grs;
  vector<TF1*> fittinos;
  
  for (unsigned i=0; i<file.size(); i++)
  {
    TFile *f = new TFile((fold+file.at(i)).c_str());
    TCanvas *c = (TCanvas*)f->Get(nameCanvas);
    grs.push_back((TGraphErrors*)c->GetListOfPrimitives()->FindObject(nameOBJ));
    if (!doRMS) fittinos.push_back( (TF1*)grs[i]->GetFunction("fittino"));
  }
  for (unsigned i=0; i<fittinos.size(); i++)
  {
    fittinos.at(i)->SetLineWidth(0);
  }
   
  TCanvas c; 
  c.cd(); 
  c.SetGrid(); 
  TMultiGraph *mg = new TMultiGraph();
  TLegend *leg;
  leg = new TLegend(0.58,0.75,0.88,0.88);
  int color[3]= {kBlue+1,kGreen+2}; 
  //int color[3]= {kBlue+1,kRed+1,kGreen+2}; 
  //int color[3]= {kViolet+1,kGreen+2}; 
  for (unsigned i=0; i<grs.size(); i++)
  {
    Color_t cl = Color_t(color[i])  ;
    grs.at(i)->SetMarkerColor(cl);
    grs.at(i)->SetMarkerStyle(21);
    mg->Add(grs.at(i));
    leg->AddEntry(grs.at(i),(label.at(i)).c_str(),"lpe");
  }


  mg->Draw("ap");
  mg->SetTitle("; Time(day/month); "+yaxis);
  mg->GetYaxis()->SetRangeUser(1.497e+9,1.511e+9);
  mg->GetYaxis()->SetRangeUser(Yrange[0],Yrange[1]);
  mg->GetXaxis()->SetTimeDisplay(1);
  mg->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");


  
  leg->Draw("same");
  c.SaveAs("cfr_"+title+".png");
  c.SaveAs("cfr_"+title+".pdf");
  c.SaveAs("cfr_"+title+".root");
  system(("mv cfr*.root "+fold).c_str());
  system(("mv *.png "+fold).c_str());
  system(("mv *.pdf "+fold).c_str());
}




void doSuperHisto(string fold, vector<string> file, TString nameCanvas,  TString nameOBJ, TString title, vector<string> label)
{
  vector<TH1F*> hs;

  for (unsigned i=0; i<file.size(); i++)
  {
  TFile *f = new TFile((fold+file.at(i)).c_str());
  TCanvas *c = (TCanvas*)f->Get(nameCanvas);
  hs.push_back((TH1F*)c->GetListOfPrimitives()->FindObject(nameOBJ));
  }
  

  TCanvas c; 
  c.cd(); 
  TLegend leg;
  //int color[3]= {kViolet+1,kGreen+2}; 
  //int color[3]= {kBlue+1,kRed+1,kGreen+2}; 
  int color[2]= {kBlue+1,kGreen+2}; 
  std::stringstream ss[3];
  for (unsigned i=0; i<hs.size(); i++)
  {
  Color_t cl = Color_t(color[i])  ;
  hs.at(i)->SetLineColor(cl);
  hs.at(i)->SetLineWidth(3);
  double RMS = hs.at(i)->GetRMS() ;
  ss[i]<<RMS<<"";

  leg.AddEntry(hs.at(i),(label.at(i)).c_str(),"l");
  }

 double max;
  if (file.size()>2)
  {
  max = hs.at(1)->GetBinContent(hs.at(1)->GetMaximumBin());
  if(hs.at(2)->GetBinContent(hs.at(2)->GetMaximumBin()) > max )
    max = hs.at(2)->GetBinContent(hs.at(2)->GetMaximumBin());
  }
  else 
  {
  max = hs.at(1)->GetBinContent(hs.at(1)->GetMaximumBin());
  }

  hs.at(0)->SetTitle("; Slope (1/s); Number of harnesses");
  hs.at(0)->GetYaxis()->SetRangeUser(0.,1.1*max);
  hs.at(0)->Draw("histo");
  hs.at(1)->Draw("histo same");
  if (file.size()>2)hs.at(2)->Draw("histo same");

 
  
  TText *t1 = new TText(.15,.78,("Std Dev = "+ss[0].str()).c_str());
  t1->SetNDC();
  t1->SetTextColor(kBlue+1);
  t1->SetTextFont(43);
  t1->SetTextSize(25);
  t1->Draw("same");
  TText *t2 = new TText(.15,.7,("Std Dev = "+ss[1].str()).c_str());
  t2->SetNDC();
  //t2->SetTextColor(kRed+1);
  t2->SetTextColor(kGreen+2);
  t2->SetTextFont(43);
  t2->SetTextSize(25);
  t2->Draw("same");
  if (file.size() > 2)
  {
  TText *t3 = new TText(.15,.62,("Std Dev = "+ss[2].str()).c_str());
  t3->SetNDC();
  t3->SetTextColor(kGreen+2);
  t3->SetTextFont(43);
  t3->SetTextSize(25);
  t3->Draw("same");
  }
  
  leg.Draw("same");
  c.SaveAs("cfr_"+title+".png");
  c.SaveAs("cfr_"+title+".pdf");
  c.SaveAs("cfr_"+title+".root");
     system(("mv cfr_*.root "+fold).c_str());
    system(("mv *.png "+fold).c_str());
    system(("mv *.pdf "+fold).c_str());
}








int main(int argc, char *argv[])
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetFillColor(-1);

  if (argc < 4 )
  {
    cout << "Missing some informations, please provide : ./superimpose input_fold method task "<< endl;
    return 0;
  }
  string input_fold=argv[1];
  string method=argv[2];
  string task = argv[3];
  //vector<string> type = argv[4];

  string fold_base ="/eos/user/f/fcetorel/www/PhiSym/eflow/"+input_fold+"/";
  
  string fold_slope = fold_base+"histo_slope/"+method+"/"; 
  string fold_fit = fold_base+"fit/"+method+"/"; 
  string fold_RMS = fold_base+"RMS/"+method+"/"; 
  string fold_cfr = fold_base+"cfr/"+method+"/"; 
  vector<double> yrange;
  
  system(("mkdir -p "+fold_cfr).c_str());
  
  //vector<string> legend_label = {"not corrected","corrected w Eflow","corrected w EFlow + Electrons"};
  vector<string> legend_label = {"not corrected","corrected w EFlow + Electrons"};
 // for (auto it = begin (type); it != end (type); ++it) 
 // {
    
 //   legend_label.push_back(label.at());
 //  } 
  
  vector<string> harness = GetHarnessRanges();
  if (task == "cfr")
  {
  
    for (unsigned  i = 1; i < harness.size(); i++ )
    {
     yrange={0.94,1.04};
     doSuperTGraph(fold_fit,{"fit_"+harness.at(i)+"norm_noCorr.root","fit_"+harness.at(i)+"norm_ZeeCorr.root"}, "c1", "gr","E/p",yrange, harness.at(i), legend_label, false);
     system(("mv "+fold_fit+"cfr* "+fold_cfr).c_str());
 
 }

  }

  else   if (task == "RMS" )
  {
    yrange = {0,0.03};
    doSuperTGraph(fold_RMS,{"RMS_mod4_noCorr.root","RMS_mod4_ZeeCorr.root"},  "c1", "gr","Spread",yrange, "RMS_mod4", legend_label, true); 
    doSuperTGraph(fold_RMS,{"RMS_mod3_noCorr.root","RMS_mod3_ZeeCorr.root"},  "c1", "gr","Spread",yrange, "RMS_mod3", legend_label,  true); 
    doSuperTGraph(fold_RMS,{"RMS_mod2_noCorr.root","RMS_mod2_ZeeCorr.root"},  "c1", "gr","Spread",yrange, "RMS_mod2", legend_label,  true); 
    doSuperTGraph(fold_RMS,{"RMS_mod01_noCorr.root","RMS_mod01_ZeeCorr.root"},  "c1", "gr","Spread",yrange, "RMS_mod01", legend_label,  true); 
    doSuperTGraph(fold_RMS,{"RMS_noCorr.root","RMS_ZeeCorr.root"},  "c1", "gr","Spread", yrange,"RMS", legend_label, true); 


  }
  else   if (task == "slope" )
  {
  
    doSuperHisto(fold_slope,{"inclusive_norm_noCorr.root","inclusive_norm_ZeeCorr.root"},  "c1", "histo_slope","inclusive_norm",legend_label); 
    doSuperHisto(fold_slope,{"mod01_norm_noCorr.root","mod01_norm_ZeeCorr.root"},  "c1", "histo_slope_mod01","mod01_norm",legend_label); 
    doSuperHisto(fold_slope,{"mod2_norm_noCorr.root","mod2_norm_ZeeCorr.root"},  "c1", "histo_slope_mod2","mod2_norm",legend_label); 
    doSuperHisto(fold_slope,{"mod3_norm_noCorr.root","mod3_norm_ZeeCorr.root"},  "c1", "histo_slope_mod3","mod3_norm",legend_label); 
    doSuperHisto(fold_slope,{"mod4_norm_noCorr.root","mod4_norm_ZeeCorr.root"},  "c1", "histo_slope_mod4","mod4_norm",legend_label); 
  }


 
}






