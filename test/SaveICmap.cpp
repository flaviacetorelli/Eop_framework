


/*
 g++ -Wall -o SaveICmap `root-config --cflags --glibs` -L $ROOTSYS/lib  SaveICmap.cpp
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


int main(int argc, char *argv[])
{
    gStyle -> SetOptStat(0);
   cout << ">>>>>>>> Opening file :";
    TFile* f = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eflow_2/CMSSW_10_6_1/src/PhiSym/EcalCalibAlgos/macros/history_EB_eflow_2018_Rereco_2d.root","read");
    TTree *ZeroBias= (TTree*)f->Get("eb");
   cout << f->GetName() << endl;
    
    int nIOV = 62; //111 for 1 day, 62 for 2 days
    int ieta = 0; 
    int iphi = 0; 
    int minRun[nIOV] = {0}; 
    int msxRun[nIOV] = {0}; 
    int firstRun[nIOV] = {0}; 
    int lastRun[nIOV] = {0}; 
    int firstLumi[nIOV] = {0}; 
    int lastLumi[nIOV] = {0};
    float ic_ratio_eflow[nIOV] = {0.}; 
   
    TH2D  *map_ic_xiov[nIOV]; 

      cout << ">>>>>>>> Filling IC maps " << endl;
    for (int i=0; i<nIOV; i++) // TH2D range is set compatible with Eop
    {
    map_ic_xiov[i] = new TH2D(("map_ic_xiov_"+to_string(i)).c_str(), "PhiSym IC/IC_{0}s map xiov;#it{i#phi};#it{i#eta}", 360, 1, 361, 171, -85, 86);
    }

    ZeroBias->SetBranchAddress("ieta",&ieta);
    ZeroBias->SetBranchAddress("ic_ratio_eflow",ic_ratio_eflow);
    ZeroBias->SetBranchAddress("iphi",&iphi);
    ZeroBias->SetBranchAddress("firstRun",firstRun);
    ZeroBias->SetBranchAddress("lastRun",lastRun);
    ZeroBias->SetBranchAddress("firstLumi",firstLumi);
    ZeroBias->SetBranchAddress("lastLumi",lastLumi);

    int n_entries = ZeroBias->GetEntries(); 
    for (int i=1; i< n_entries;  i++)
    {
      float ic_ratio_eflow_avg = 0.;
      ZeroBias -> GetEntry(i);

      for (int l = 0 ; l < nIOV; l++ )
      {  
        if (ic_ratio_eflow[l] >= 0) map_ic_xiov[l]->Fill(iphi,ieta,ic_ratio_eflow[l]);
        else  map_ic_xiov[l]->Fill(iphi,ieta,0.);
        ic_ratio_eflow_avg += ic_ratio_eflow[l];
      }

    }
    
   cout << ">>>>>>>> Saving IC maps" << endl;
   TFile *MyFile = new TFile("ICch_EB_2018Rereco_2d.root","RECREATE");
   TString filename =  MyFile->GetName();

    for (int i = 0; i<nIOV; i++)
    {
    
      map_ic_xiov[i]->SetContour(100000);
      map_ic_xiov[i]->SetAxisRange(0.98, 1.02, "Z");

      map_ic_xiov[i] -> SetTitle((to_string(firstRun[i])+"-"+to_string(firstLumi[i])+"_"+to_string(lastRun[i])+"-"+to_string(lastLumi[i])).c_str());
      //cout << map_ic_xiov[i] -> GetTitle() << endl;
      map_ic_xiov[i] -> SetName(("map_ic_"+to_string(i)).c_str());
      //map_ic_xiov[i] -> Write(("map_ic_"+to_string(firstRun[i])+"_"+to_string(lastRun[i])).c_str());
      map_ic_xiov[i] -> Write(("map_ic_"+to_string(i)).c_str());


    }

   cout << ">>>>>>>> Transferring IC maps in data/" << MyFile->GetName() << endl;

   system("mv " + filename + " /afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/data");


   MyFile -> Close();

}
