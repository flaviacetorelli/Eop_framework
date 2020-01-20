/*
 g++ -Wall -o superimpose `root-config --cflags --glibs` -L $ROOTSYS/lib  superimpose.cpp
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


void doSuperTGraph(TString file1, TString file2, TString nameCanvas,  TString nameOBJ, TString title)
{

  TFile f1(file1);
  TCanvas *c1 = (TCanvas*)f1.Get(nameCanvas);
  TGraphErrors *gr1 = (TGraphErrors*)c1->GetListOfPrimitives()->FindObject(nameOBJ);

  TFile f2(file2);
  TCanvas *c2 = (TCanvas*)f2.Get(nameCanvas);
  TGraphErrors *gr2 = (TGraphErrors*)c2->GetListOfPrimitives()->FindObject(nameOBJ);

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
  mg->SetTitle("; Time(day/month); Spread");
  mg->GetXaxis()->SetTimeDisplay(1);
  mg->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  mg->GetYaxis()->SetRangeUser(0.,0.03);

  TLegend leg; 
  leg.AddEntry(gr1,"corrected","lpe");
  leg.AddEntry(gr2,"not corrected","lpe");
  leg.Draw("same");
  c.SaveAs("cfr_"+title+".png");
  c.SaveAs("cfr_"+title+".pdf");
  c.SaveAs("cfr_"+title+".root");
system("mv cfr*.root /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS");
system("mv *.png /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS");
system("mv *.pdf /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS");
}


void doSuperTGraph(TString file1, TString file2,TString file3, TString nameCanvas,  TString nameOBJ, TString title)
{

  TFile f1(file1);
  TCanvas *c1 = (TCanvas*)f1.Get(nameCanvas);
  TGraphErrors *gr1 = (TGraphErrors*)c1->GetListOfPrimitives()->FindObject(nameOBJ);

  TFile f2(file2);
  TCanvas *c2 = (TCanvas*)f2.Get(nameCanvas);
  TGraphErrors *gr2 = (TGraphErrors*)c2->GetListOfPrimitives()->FindObject(nameOBJ);

  TFile f3(file3);
  TCanvas *c3 = (TCanvas*)f3.Get(nameCanvas);
  TGraphErrors *gr3 = (TGraphErrors*)c3->GetListOfPrimitives()->FindObject(nameOBJ);

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
  mg->SetTitle("; Time(day/month); Spread");
  mg->GetXaxis()->SetTimeDisplay(1);
  mg->GetXaxis()->SetTimeFormat("%d/%m%F1970-01-01 00:00:00");
  mg->GetYaxis()->SetRangeUser(0.,0.03);

  TLegend leg(0.5,0.7,0.9,0.9); 
  leg.AddEntry(gr2,"not corrected","lpe");
  leg.AddEntry(gr1,"corrected w eflow","lpe");
  leg.AddEntry(gr3,"corrected w eflow+E/p","lpe");
  leg.Draw("same");
  c.SaveAs("cfr_"+title+".png");
  c.SaveAs("cfr_"+title+".pdf");
  c.SaveAs("cfr_"+title+".root");
system("mv cfr*.root /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS");
system("mv *.png /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS");
system("mv *.pdf /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS");
}

void doSuperHisto(TString file1, TString file2, TString nameCanvas,  TString nameOBJ, TString title)
{
  gStyle->SetOptStat(0);
  TFile f1(file1);
  TCanvas *c1 = (TCanvas*)f1.Get(nameCanvas);
  TH1F* h1 = (TH1F*)c1->GetListOfPrimitives()->FindObject(nameOBJ);

  TFile f2(file2);
  TCanvas *c2 = (TCanvas*)f2.Get(nameCanvas);
  TH1F* h2 = (TH1F*)c2->GetListOfPrimitives()->FindObject(nameOBJ);

  TCanvas c; 
  c.cd(); 

  h1->SetLineColor(kRed+1);
  h2->SetLineColor(kBlue+1);
  h1->SetLineWidth(3);
  h2->SetLineWidth(3);


  double max = h1->GetBinContent(h1->GetMaximumBin());
  if(h2->GetBinContent(h2->GetMaximumBin()) > max )
    max = h2->GetBinContent(h2->GetMaximumBin());




  h1->GetYaxis()->SetRangeUser(0.,1.1*max);

  h1->SetTitle("; Slope (1/s); Number of harnesses");
  h1->Draw("histo");
  h2->Draw("histo same");
  std::stringstream ss1,ss2,ss3,ss4;
  double RMS1 = h1->GetRMS() ;
  double RMS2 = h2->GetRMS() ;

  ss1<<RMS1<<"";
  ss2<<RMS2<<"";

  double mean1 = h1->GetMean() ;
  double mean2 = h2->GetMean() ;
  ss3<<mean1<<"";
  ss4<<mean2<<"";




  
  
  TText *t1 = new TText(.15,.78,("Std Dev = "+ss1.str()).c_str());
  t1->SetNDC();
  t1->SetTextColor(kRed);
  t1->SetTextFont(43);
  t1->SetTextSize(25);
  t1->Draw("same");
  TText *t2 = new TText(.15,.64,("Std Dev = "+ss2.str()).c_str());
  t2->SetNDC();
  t2->SetTextColor(kBlue);
  t2->SetTextFont(43);
  t2->SetTextSize(25);
  t2->Draw("same");
  TText *t3 = new TText(.15,.71,("Mean = "+ss3.str()).c_str());
  t3->SetNDC();
  t3->SetTextColor(kRed);
  t3->SetTextFont(43);
  t3->SetTextSize(25);
  t3->Draw("same");
  TText *t4 = new TText(.15,.57,("Mean = "+ss4.str()).c_str());
  t4->SetNDC();
  t4->SetTextColor(kBlue);
  t4->SetTextFont(43);
  t4->SetTextSize(25);
  t4->Draw("same");


  TLegend leg; 
  leg.AddEntry(h2,"Range 0.8-1.4","l");
  leg.AddEntry(h1,"Range 0.1-2.0","l");
  leg.Draw("same");
  c.SaveAs("cfr_"+title+".png");
  c.SaveAs("cfr_"+title+".pdf");
  c.SaveAs("cfr_"+title+".root");
  system("mv cfr*.root /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
  system("mv *.png /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
  system("mv *.pdf /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
}

void doSuperHisto(TString file1, TString file2, TString file3,  TString nameCanvas,  TString nameOBJ, TString title)
{
  gStyle->SetOptStat(0);
  TFile f1(file1);
  TCanvas *c1 = (TCanvas*)f1.Get(nameCanvas);
  TH1F* h1 = (TH1F*)c1->GetListOfPrimitives()->FindObject(nameOBJ);

  TFile f2(file2);
  TCanvas *c2 = (TCanvas*)f2.Get(nameCanvas);
  TH1F* h2 = (TH1F*)c2->GetListOfPrimitives()->FindObject(nameOBJ);

  TFile f3(file3);
  TCanvas *c3 = (TCanvas*)f3.Get(nameCanvas);
  TH1F* h3 = (TH1F*)c3->GetListOfPrimitives()->FindObject(nameOBJ);

  TCanvas c; 
  c.cd(); 
  h1->SetLineColor(kRed+1);
  h2->SetLineColor(kBlue+1);
  h3->SetLineColor(kGreen+1);
  h1->SetLineWidth(3);
  h2->SetLineWidth(3);
  h3->SetLineWidth(3);

  double max = h1->GetBinContent(h1->GetMaximumBin());
  if(h3->GetBinContent(h3->GetMaximumBin()) > max )
    max = h3->GetBinContent(h3->GetMaximumBin());

  h1->SetTitle("; Slope (1/s); Number of harnesses");
  h1->GetYaxis()->SetRangeUser(0.,1.1*max);
  h1->Draw("histo");
  h2->Draw("histo same");
  h3->Draw("histo same");

  double RMS1 = h1->GetRMS() ;
  double RMS2 = h2->GetRMS() ;
  double RMS3 = h3->GetRMS() ;
  std::stringstream ss1;
  std::stringstream ss2;
  std::stringstream ss3;
  ss1<<RMS1<<"";
  ss2<<RMS2<<"";
  ss3<<RMS3<<"";



  
  
  TText *t1 = new TText(.15,.78,("Std Dev = "+ss1.str()).c_str());
  t1->SetNDC();
  t1->SetTextColor(kRed+1);
  t1->SetTextFont(43);
  t1->SetTextSize(25);
  t1->Draw("same");
  TText *t2 = new TText(.15,.7,("Std Dev = "+ss2.str()).c_str());
  t2->SetNDC();
  t2->SetTextColor(kBlue+1);
  t2->SetTextFont(43);
  t2->SetTextSize(25);
  t2->Draw("same");
  TText *t3 = new TText(.15,.62,("Std Dev = "+ss3.str()).c_str());
  t3->SetNDC();
  t3->SetTextColor(kGreen+1);
  t3->SetTextFont(43);
  t3->SetTextSize(25);
  t3->Draw("same");


  TLegend leg;
  leg.AddEntry(h2,"not corrected","l");
  leg.AddEntry(h1,"corrected w eflow","l");
  leg.AddEntry(h3,"corrected w eflow + E/p","l");
  leg.Draw("same");
  c.SaveAs("cfr_"+title+".png");
  c.SaveAs("cfr_"+title+".pdf");
  c.SaveAs("cfr_"+title+".root");
  system("mv cfr*.root /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
  system("mv *.png /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
  system("mv *.pdf /eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope");
}
int main(int argc, char *argv[])
{

TString fold_slope = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/"; 
TString fold_RMS = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS/"; 
/*
doSuperTGraph(fold_RMS+"RMS_mod01.root", fold_RMS+"RMS_mod01noCorr.root",fold_RMS+"RMS_mod01test.root", "c1", "gr", "RMS_mod01_new"); 
doSuperTGraph(fold_RMS+"RMS_mod2.root", fold_RMS+"RMS_mod2noCorr.root", fold_RMS+"RMS_mod2test.root", "c1", "gr", "RMS_mod2_new"); 
doSuperTGraph(fold_RMS+"RMS_mod3.root", fold_RMS+"RMS_mod3noCorr.root", fold_RMS+"RMS_mod3test.root", "c1", "gr", "RMS_mod3_new"); 
doSuperTGraph(fold_RMS+"RMS_mod4.root", fold_RMS+"RMS_mod4noCorr.root", fold_RMS+"RMS_mod4test.root", "c1", "gr", "RMS_mod4_new"); 
doSuperTGraph(fold_RMS+"RMS.root", fold_RMS+"RMSnoCorr.root",fold_RMS+"RMStest.root", "c1", "gr", "RMS_new"); 

doSuperTGraph(fold_RMS+"RMS_mod01.root", fold_RMS+"RMS_mod01noCorr.root",fold_RMS+"RMS_mod01test.root", "c1", "gr", "RMS_mod01_new"); 
doSuperTGraph(fold_RMS+"RMS_mod2.root", fold_RMS+"RMS_mod2noCorr.root", fold_RMS+"RMS_mod2test.root", "c1", "gr", "RMS_mod2_new"); 
doSuperTGraph(fold_RMS+"RMS_mod3.root", fold_RMS+"RMS_mod3noCorr.root", fold_RMS+"RMS_mod3test.root", "c1", "gr", "RMS_mod3_new"); 
doSuperTGraph(fold_RMS+"RMS_mod4.root", fold_RMS+"RMS_mod4noCorr.root", fold_RMS+"RMS_mod4test.root", "c1", "gr", "RMS_mod4_new"); 
doSuperTGraph(fold_RMS+"RMS.root", fold_RMS+"RMSnoCorr.root",fold_RMS+"RMStest.root", "c1", "gr", "RMS_new"); 
*/
doSuperHisto(fold_slope+"histo_slope_norm1test.root",fold_slope+"histo_slope_normtest.root", "c1", "histo_slope", "histo_slope_new"); 
doSuperHisto(fold_slope+"histo_slope_mod01_norm1test.root",fold_slope+"histo_slope_mod01_normtest.root", "c1", "histo_slope_mod01", "histo_slope_mod01_new"); 
doSuperHisto(fold_slope+"histo_slope_mod2_norm1test.root",fold_slope+"histo_slope_mod2_normtest.root", "c1", "histo_slope_mod2", "histo_slope_mod2_new"); 
doSuperHisto(fold_slope+"histo_slope_mod3_norm1test.root",fold_slope+"histo_slope_mod3_normtest.root", "c1", "histo_slope_mod3", "histo_slope_mod3_new"); 
doSuperHisto(fold_slope+"histo_slope_mod4_norm1test.root",fold_slope+"histo_slope_mod4_normtest.root", "c1", "histo_slope_mod4", "histo_slope_mod4_new"); 


}






