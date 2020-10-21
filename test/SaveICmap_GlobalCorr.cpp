/*
 g++ -Wall -o SaveICmap_GlobalCorr `root-config --cflags --glibs` -L $ROOTSYS/lib  SaveICmap_GlobalCorr.cpp
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
    
    //leggo le info da riga di comando
    if (argc < 4) 
    {
      cout << "Missing some informations " << argv[0] << " inputfile(complete path, map of eflow corr) tag(yearReco_iovduration)  method(mean, median...) number_iov global_corr_file(complete path)" << endl ;
      exit (1) ;
    }


    TString inputfile = argv[1];
    TString tag = argv[2] ;//"2018Rereco_2d"; //in the form: yearReco_iovduration
    TString method = argv[3] ; //e.g. mee_median , Eop_median
    cout << ">>>>>>>> Opening file :";
    TFile* inICfile = new TFile(inputfile,"READ");
    cout<<"> Loading IC from "<<inICfile->GetName()<< endl; 
    
    int size = atoi(argv[4]); //111 for 1 day, 62 for 2 days
    TString globalCorrFile = argv[5] ;

    std::map <std::vector<int>, TH2D*> hMap; 
    
    for (unsigned i = 0; i< size; i++) //carico gli istogrammi in una mappa
    {        
         TH2D *h= (TH2D*)inICfile->Get(("map_ic_"+to_string(i)).c_str());

         std::string title = h->GetTitle(); //the tile of the histo contains the info on the run number
         std::vector<int> run_num;  

         std::string::size_type sx,sy,sz;   // convert string into int
         run_num.push_back(stoi(title, &sx));   
         run_num.push_back(stoi(title.substr(sx+1),&sy));
         run_num.push_back(stoi(title.substr(sx+sy+2),&sz));
         run_num.push_back(stoi(title.substr(sx+sy+sz+3)));


         hMap.insert(make_pair(run_num, h));
     }
    // Open file with the global scale corrections
    TFile* inEopfile = new TFile(globalCorrFile, "READ");
    
    unsigned runmin, runmax; 
    UShort_t lsmin, lsmax; 
    float scale, scale_unc; 
 
    TTree *t= (TTree*)inEopfile->Get("ciao"); 
    t->SetBranchAddress("lsmin",&lsmin); 
    t->SetBranchAddress("lsmax", &lsmax);
    t->SetBranchAddress("runmin",&runmin); 
    t->SetBranchAddress("runmax", &runmax); 
    t->SetBranchAddress("scale_"+method, &scale); 
    t->SetBranchAddress("scale_unc_"+method, &scale_unc); 

    float norm, mean, emean; 

    t -> GetEntry(0);
    norm = scale;
    for (long ientry=0; ientry < t->GetEntries(); ientry++)
    {
       t->GetEntry(ientry);

       
       for (auto& imap : hMap )
       {
         //cout << "Try to match " << runmin << "," << runmax << " in " << "(" << imap.first.at(0) << "," << imap.first.at(2)<<")" << endl; 
         if (runmin == unsigned(imap.first.at(0)) && runmax== unsigned(imap.first.at(2)))
         {
            if (lsmin == unsigned(imap.first.at(1)) && lsmax== unsigned(imap.first.at(3)))
           {
           //cout << "Match" << endl;  
           //cout << "Before Normalization " << scale <<  " : " << "normalization" << norm << endl;
           scale = scale / norm;   
           //cout << "After Normalization " << scale << endl;
           imap.second->Scale(1./scale); 
           break;     
           }
         }
         
       }

     }


  
   TFile *outFile = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/data/ICch_EB_"+tag+"_"+method+".root","RECREATE");
 
   for (auto& imap : hMap )
   {
      imap.second -> Write();
  }


   outFile -> Close();

   cout << ">>>>>>>> Transferring IC maps with also E/p correction in data/" <<  endl;
   


}
