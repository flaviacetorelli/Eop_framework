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
    TFile* f = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/EFlow/CMSSW_10_5_0/src/PhiSym/EcalCalibAlgos/macros/history_eflow_2017_new.root");
    TTree *ZeroBias= (TTree*)f->Get("eb");
   cout << f->GetName() << endl;
    

    int ieta = 0; 
    int iphi = 0; 
    int minRun[104] = {0}; 
    int maxRun[104] = {0}; 
    int firstRun[104] = {0}; 
    int lastRun[104] = {0}; 
    float ic_ratio_eflow[104] = {0.}; 
   
    TH2D  *map_ic_xiov[104]; 

      cout << ">>>>>>>> Filling IC maps " << endl;
    for (int i=0; i<104; i++) // TH2D range is set compatible with Eop
    {
    map_ic_xiov[i] = new TH2D(("map_ic_xiov_"+to_string(i)).c_str(), "PhiSym IC/IC_{0}s map xiov;#it{i#phi};#it{i#eta}", 360, 1, 361, 171, -85, 86);
    }

    ZeroBias->SetBranchAddress("ieta",&ieta);
    ZeroBias->SetBranchAddress("ic_ratio_eflow",ic_ratio_eflow);
    ZeroBias->SetBranchAddress("iphi",&iphi);
    ZeroBias->SetBranchAddress("firstRun",firstRun);
    ZeroBias->SetBranchAddress("lastRun",lastRun);

    int n_entries = ZeroBias->GetEntries(); 
    for (int i=1; i< n_entries;  i++)
    {
      float ic_ratio_eflow_avg = 0.;
      ZeroBias -> GetEntry(i);

      for (int l = 0 ; l < 104; l++ )
      {  
        map_ic_xiov[l]->Fill(iphi,ieta,ic_ratio_eflow[l]);
        ic_ratio_eflow_avg += ic_ratio_eflow[l];
      }

    }
    
   cout << ">>>>>>>> Saving IC maps" << endl;
   TFile *MyFile = new TFile("testICEB.root","RECREATE");
   
    for (int i = 0; i<104; i++)
    {
    
      map_ic_xiov[i]->SetContour(100000);
      map_ic_xiov[i]->SetAxisRange(0.98, 1.02, "Z");

      map_ic_xiov[i] -> SetName(("map_ic_"+to_string(firstRun[i])+"_"+to_string(lastRun[i])).c_str());
      map_ic_xiov[i] -> Write(("map_ic_"+to_string(firstRun[i])+"_"+to_string(lastRun[i])).c_str());


    }

   cout << ">>>>>>>> Transferring IC maps in data/testICEB.root" << endl;

   system("mv testICEB.root /afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/data");


   MyFile -> Close();

}
