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
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

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


void doSuperTGraph(vector<string> file, TString nameCanvas,  TString nameOBJ, TString yaxis, vector<double> Yrange,   TString title, vector<string> label, TString output, bool doRMS)
{
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  vector<TGraphErrors*> grs;
  vector<TF1*> fittinos;
  for (unsigned i=0; i<file.size(); i++)
  {
  TFile *f = new TFile((file.at(i)).c_str());
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
  leg = new TLegend(0.58,0.75,0.9,0.9);
  //int color[3]= {kBlue+1,kRed+1,kGreen+2}; 
  int color[3]= {kViolet+1,kGreen+2}; 
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
system("mv cfr*.root "+output);
system("mv *.png "+output);
system("mv *.pdf "+output);
}





void doSuperHisto(vector<string> file, TString nameCanvas,  TString nameOBJ, TString title, vector<string> label, TString output)
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  vector<TH1F*> hs;
  for (unsigned i=0; i<file.size(); i++)
  {
  TFile *f = new TFile((file.at(i)).c_str());
  TCanvas *c = (TCanvas*)f->Get(nameCanvas);
  hs.push_back((TH1F*)c->GetListOfPrimitives()->FindObject(nameOBJ));
  }
  

  TCanvas c; 
  c.cd(); 
  TLegend leg;
  //int color[3]= {kBlue+1,kRed+1,kGreen+2}; 
  int color[3]= {kViolet+1,kGreen+2}; 
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
  t2->SetTextColor(kRed+1);
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
  system("mv cfr*.root "+output);
  system("mv *.png "+output);
  system("mv *.pdf "+output);
}








int main(int argc, char *argv[])
{
string method=argv[1];
string fold_EFlow = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/Eflow_Eop/mean/";
string output_EFlow = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/Eflow_Eop/templatefit/";
string fold_slope = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/histo_slope/"+method+"/"; 
string fold_fit = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/fit/templatefit/"; 
string fold_RMS = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS/"+method+"/"; 
string output_slope = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/histo_slope/"+method+"/";
string output_RMS = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/RMS/"+method+"/";

string output_cfr = "/eos/user/f/fcetorel/www/PhiSym/eflow/cfr_Eop/harness_slope/cfr/"+method+"/"; 
//vector<string> legend_label = {"not corrected","corrected w Eflow","corrected w EFlow + Eop"};
vector<string> legend_label = {"corrected w E/p","corrected w EFlow + Eop", ""};
    string har = "IEta_-45_-26_IPhi_141_150";
    //string har = "IEta_6_25_IPhi_221_230";
  vector<string> harness = GetHarnessRanges();
doSuperTGraph({fold_fit+"fit_"+har+"norm_oldCorr.root",fold_fit+"fit_"+har+"norm_EopCorr.root"}, "c1", "gr","E/p",{0.94,1.04}, har, legend_label, output_cfr+"oldCorr/", false); 
 /* for (unsigned  i = 1; i < harness.size(); i++ )
  {
doSuperTGraph({fold_fit+"fit_"+harness.at(i)+"norm_oldCorr.root",fold_fit+"fit_"+harness.at(i)+"norm_EopCorr.root"}, "c1", "gr","E/p",{0.94,1.04}, harness.at(i), legend_label, output_cfr+"oldCorr/", false); 
//doEopEFlow(fold_EFlow+"cfr_"+harness.at(i)+".root",fold_fit+"fit_"+harness.at(i)+"norm_noCorr.root", "c1", "gr", harness.at(i), output_EFlow); 
    // doSuperTGraph({fold_fit+"fit_"+harness.at(i)+"norm_noCorr.root",fold_fit+"fit_"+harness.at(i)+"norm_EFlowCorr.root"}, "c1", "gr","E/p",{0.94,1.04}, harness.at(i), legend_label, output_cfr, false); 
     //doSuperTGraph({fold_fit+"fit_"+harness.at(i)+"norm_noCorr.root",fold_fit+"fit_"+harness.at(i)+"norm_EFlowCorr.root",fold_fit+"fit_"+harness.at(i)+"norm_EopCorr.root"}, "c1", "gr","E/p",{0.94,1.04}, harness.at(i), legend_label, output_cfr+"wEopCorr/", false); 
  }







doSuperTGraph({fold_RMS+"RMS_mod4_noCorr.root",fold_RMS+"RMS_mod4_EFlowCorr.root"},  "c1", "gr","Spread", "RMS_mod4", legend_label, output_RMS, false); 
doSuperTGraph({fold_RMS+"RMS_mod3_noCorr.root",fold_RMS+"RMS_mod3_EFlowCorr.root"},  "c1", "gr","Spread", "RMS_mod3", legend_label, output_RMS, false); 
doSuperTGraph({fold_RMS+"RMS_mod2_noCorr.root",fold_RMS+"RMS_mod2_EFlowCorr.root"},  "c1", "gr","Spread", "RMS_mod2", legend_label, output_RMS, false); 
doSuperTGraph({fold_RMS+"RMS_mod01_noCorr.root",fold_RMS+"RMS_mod01_EFlowCorr.root"},  "c1", "gr","Spread", "RMS_mod01", legend_label, output_RMS, false); 
doSuperTGraph({fold_RMS+"RMS_noCorr.root",fold_RMS+"RMS_EFlowCorr.root"},  "c1", "gr","Spread", "RMS", legend_label, output_RMS, false); 

doSuperHisto({fold_slope+"inclusive_norm_noCorr.root",fold_slope+"inclusive_norm_EFlowCorr.root"},  "c1", "histo_slope","inclusive_norm",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod01_norm_noCorr.root",fold_slope+"mod01_norm_EFlowCorr.root"},  "c1", "histo_slope_mod01","mod01_norm",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod2_norm_noCorr.root",fold_slope+"mod2_norm_EFlowCorr.root"},  "c1", "histo_slope_mod2","mod2_norm",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod3_norm_noCorr.root",fold_slope+"mod3_norm_EFlowCorr.root"},  "c1", "histo_slope_mod3","mod3_norm",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod4_norm_noCorr.root",fold_slope+"mod4_norm_EFlowCorr.root"},  "c1", "histo_slope_mod4","mod4_norm",legend_label, output_slope); 

doSuperTGraph({fold_RMS+"RMS_mod4_noCorr.root",fold_RMS+"RMS_mod4_EFlowCorr.root",fold_RMS+"RMS_mod4_EopCorr.root"},  "c1", "gr","Spread",{0.,0.03}, "RMS_mod4_all", legend_label, output_RMS); 
doSuperTGraph({fold_RMS+"RMS_mod3_noCorr.root",fold_RMS+"RMS_mod3_EFlowCorr.root",fold_RMS+"RMS_mod3_EopCorr.root"},  "c1", "gr","Spread",{0.,0.03}, "RMS_mod3_all", legend_label, output_RMS); 
doSuperTGraph({fold_RMS+"RMS_mod2_noCorr.root",fold_RMS+"RMS_mod2_EFlowCorr.root",fold_RMS+"RMS_mod2_EopCorr.root"},  "c1", "gr","Spread",{0.,0.03}, "RMS_mod2_all", legend_label, output_RMS); 
doSuperTGraph({fold_RMS+"RMS_mod01_noCorr.root",fold_RMS+"RMS_mod01_EFlowCorr.root",fold_RMS+"RMS_mod01_EopCorr.root"},  "c1", "gr","Spread",{0.,0.03}, "RMS_mod01_all", legend_label, output_RMS); 
doSuperTGraph({fold_RMS+"RMS_noCorr.root",fold_RMS+"RMS_EFlowCorr.root",fold_RMS+"RMS_EopCorr.root"},  "c1", "gr","Spread",{0.,0.03},"RMS_all", legend_label, output_RMS); 

doSuperHisto({fold_slope+"inclusive_norm_noCorr.root",fold_slope+"inclusive_norm_EFlowCorr.root", fold_slope+"inclusive_norm_EopCorr.root"},  "c1", "histo_slope","inclusive_norm_all",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod01_norm_noCorr.root",fold_slope+"mod01_norm_EFlowCorr.root",fold_slope+"mod01_norm_EopCorr.root"},  "c1", "histo_slope_mod01","mod01_norm_all",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod2_norm_noCorr.root",fold_slope+"mod2_norm_EFlowCorr.root",fold_slope+"mod2_norm_EopCorr.root"},  "c1", "histo_slope_mod2","mod2_norm_all",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod3_norm_noCorr.root",fold_slope+"mod3_norm_EFlowCorr.root", fold_slope+"mod3_norm_EopCorr.root"},  "c1", "histo_slope_mod3","mod3_norm_all",legend_label, output_slope); 
doSuperHisto({fold_slope+"mod4_norm_noCorr.root",fold_slope+"mod4_norm_EFlowCorr.root", fold_slope+"mod4_norm_EopCorr.root"},  "c1", "histo_slope_mod4","mod4_norm_all",legend_label, output_slope); 
*/
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

doSuperHisto(fold_slope+"histo_slope_norm1test.root",fold_slope+"histo_slope_normtest.root", "c1", "histo_slope", "histo_slope_new"); 
doSuperHisto(fold_slope+"histo_slope_mod01_norm1test.root",fold_slope+"histo_slope_mod01_normtest.root", "c1", "histo_slope_mod01", "histo_slope_mod01_new"); 
doSuperHisto(fold_slope+"histo_slope_mod2_norm1test.root",fold_slope+"histo_slope_mod2_normtest.root", "c1", "histo_slope_mod2", "histo_slope_mod2_new"); 
doSuperHisto(fold_slope+"histo_slope_mod3_norm1test.root",fold_slope+"histo_slope_mod3_normtest.root", "c1", "histo_slope_mod3", "histo_slope_mod3_new"); 
doSuperHisto(fold_slope+"histo_slope_mod4_norm1test.root",fold_slope+"histo_slope_mod4_normtest.root", "c1", "histo_slope_mod4", "histo_slope_mod4_new"); 
*/

}






