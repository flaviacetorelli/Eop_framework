/*
 g++ -Wall -o RunDivide `root-config --cflags --glibs` -L $ROOTSYS/lib  RunDivide.cpp
 */


#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"

#include "TChain.h"
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
    cout << ">>>>>>>> Opening file : ";
    TFile* f = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eflow_2/CMSSW_10_6_1/src/PhiSym/EcalCalibAlgos/macros/history_EB_eflow_2018_Rereco_2d.root");
    TTree *ZeroBias= (TTree*)f->Get("eb");
    cout << f->GetName() << endl;

    unsigned timemax = 9e10;  
    unsigned timemin = 0; 
    unsigned runmin = 0; 
    unsigned runmax = 0; 
    unsigned short lsmin = 0; 
    unsigned short lsmax = 0;
    int Nev = 0; 
    int nIOV = 113;
    int firstLumi[nIOV] = {0}; 
    int lastLumi[nIOV] = {0}; 
    int firstRun[nIOV] = {0}; 
    int lastRun[nIOV] = {0}; 
    int n_events[nIOV] = {0}; 
    int avg_time[nIOV] = {0};


    ZeroBias->SetBranchAddress("firstLumi",firstLumi);
    ZeroBias->SetBranchAddress("lastLumi",lastLumi);
    ZeroBias->SetBranchAddress("firstRun",firstRun);
    ZeroBias->SetBranchAddress("lastRun",lastRun);
    ZeroBias->SetBranchAddress("n_events",n_events);
    ZeroBias->SetBranchAddress("avg_time",avg_time);
    ZeroBias->GetEntry(1);



  // -------------------------------------------  

   cout << ">>>>>>>> Saving TimeBin in ./test" << endl;

    for ( int i = 0 ; i < nIOV; i++ )
    {
      TFile *MyFile = new TFile(("runranges_"+to_string(firstRun[i])+"-"+to_string(firstLumi[i])+"_"+to_string(lastRun[i])+"-"+to_string(lastLumi[i])+".root").c_str(),"RECREATE");
      TTree* out = new TTree("ciao","ciao");    
      out->Branch("runmin", &runmin, "runmin/i");
      out->Branch("runmax", &runmax, "runmax/i");
      out->Branch("lsmin", &lsmin, "lsmin/s");
      out->Branch("lsmax", &lsmax, "lsmax/s");
      out->Branch("timemin", &timemin, "timemin/i");
      out->Branch("timemax", &timemax, "timemax/i");
      out->Branch("Nev", &Nev, "Nev/I"); 
      runmin = unsigned(firstRun[i]);
      runmax = unsigned(lastRun[i]);
      lsmin = short(firstLumi[i]);
      lsmax = short (lastLumi[i]);
      timemin = unsigned (avg_time[i]); 
      timemax = 9e10; 
      Nev = unsigned (n_events[i]);
      out->Fill();
      out->Write(); 
      MyFile -> Close();
    }


}
