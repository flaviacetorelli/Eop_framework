


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
    if (argc < 3) 
    {
      cout << "usage ./" << argv[0] << "inputfile tag number_iov" << endl ;
      exit (1) ;
    }


    string inputfile = argv[1];
    //string inputfile = "/afs/cern.ch/work/f/fcetorel/private/work2/Eflow_2/CMSSW_10_6_1/src/PhiSym/EcalCalibAlgos/macros/history_EB_eflow_2018_Rereco_2d.root";
    string tag = argv[2] ;//"2018Rereco_2d"; //in the form: yearReco_iovduration
    cout << ">>>>>>>> Opening file :";
    TFile* f = new TFile(inputfile.c_str(),"read");
    TTree *ZeroBias= (TTree*)f->Get("eb");
    cout << f->GetName() << endl;
    
    int nIOV = atoi(argv[3]); //111 for 1 day, 62 for 2 days
    cout << nIOV << endl;    
    int ieta = 0; 
    int iphi = 0; 
    int minRun[nIOV]; 
    int msxRun[nIOV]; 
    int firstRun[nIOV]; 
    int lastRun[nIOV]; 
    int firstLumi[nIOV]; 
    int lastLumi[nIOV];
    float ic_ratio_eflow[nIOV]; 
   
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
    for (int i=1; i< n_entries;  i++) //fill the histo maps
    {
      //float ic_ratio_eflow_avg = 0.;
      ZeroBias -> GetEntry(i);

      for (int l = 0 ; l < nIOV; l++ )
      {  
        if (ic_ratio_eflow[l] >= 0) map_ic_xiov[l]->Fill(iphi,ieta,ic_ratio_eflow[l]);
        else  map_ic_xiov[l]->Fill(iphi,ieta,0.);
        //ic_ratio_eflow_avg += ic_ratio_eflow[l];
      }

    }
    
    cout << ">>>>>>>> Saving IC maps" << endl;
    TFile *icmap_file = new TFile(("ICch_EB_"+tag+".root").c_str(),"RECREATE");
    TString filename =  icmap_file ->GetName();

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

   cout << ">>>>>>>> Transferring IC maps in data/" << filename << endl;

   system("mv " + filename + " /afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/data");


   icmap_file -> Close();
   
   //create a dictionary that must be read by the Eop framework to create IOV dependent ic list
   ofstream iovdict_file;
   iovdict_file.open(("MapDictionary_"+tag+".txt").c_str());
   for (int i = 0; i<nIOV; i++)
   {
     string icmapname = "map_ic_"+to_string(i);
     //cout << icmapname << endl;
     // printf("%s",icmapname.c_str());
     
     iovdict_file << firstRun[i] << "\t" ;
     iovdict_file << firstLumi[i] << "\t" ;
     iovdict_file << lastRun[i] << "\t" ;
     iovdict_file << lastLumi[i] << "\t" ;
     iovdict_file << icmapname << "\n" ;
     //fprintf(iovdict_file,"%i\t%i\t%i\t%i\t%s\n",firstRun[i],firstLumi[i],lastRun[i],lastLumi[i],icmapname.c_str());
   }
   iovdict_file.close();
}
