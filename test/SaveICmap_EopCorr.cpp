/*
 g++ -Wall -o SaveICmap_EopCorr `root-config --cflags --glibs` -L $ROOTSYS/lib  SaveICmap_EopCorr.cpp
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

    TFile* inICfile = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/data/testICEB1.root", "READ");
    unsigned size = 104; //# of IOVs
    cout<<"> Loading IC from "<<inICfile->GetName()<<"/"<<endl; 

    std::map <std::vector<int>, TH2D*> hMap; 

    for (unsigned i = 0; i< size; i++) //carico gli istogrammi in una mappa
    {        
         TH2D *h= (TH2D*)inICfile->Get(("map_ic_"+to_string(i)).c_str());

         std::string title = h->GetTitle(); //the tile of the histo contains the info on the run number
         std::vector<int> run_num;  

         std::string::size_type sz;   // convert string into int
         run_num.push_back(stoi(title, &sz));   
         run_num.push_back(stoi(title.substr(sz+1)));

         hMap.insert(make_pair(run_num, h));
     }
     
     //Apro il file di E/p su tutto il Barrel per correggere la scala 
    TFile* inEopfile = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/all/scaleMonitor_all.root", "READ");
    unsigned runmin, runmax; 
    float scale_Eop_mean, scale_err_Eop_mean; 
    TTree *t= (TTree*)inEopfile->Get("ciao"); 
    t->SetBranchAddress("runmin",&runmin); 
    t->SetBranchAddress("runmax", &runmax); 
    t->SetBranchAddress("scale_Eop_mean", &scale_Eop_mean); 
    t->SetBranchAddress("scale_err_Eop_mean", &scale_err_Eop_mean); 

    float norm, mean, emean; 
    for (int i= 0; i<5; i++) // normalization to the w mean of first 5 points
    {
       t->GetEntry(i);
       mean += scale_Eop_mean*(1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
       emean += (1./scale_err_Eop_mean)*(1./scale_err_Eop_mean);
       //cout << "This is mean" << mean << endl; 
    }
    norm = mean/emean;
   // cout << "First point for normalization "<< norm << endl; 


    for (long ientry=0; ientry < t->GetEntries(); ientry++)
    {
       t->GetEntry(ientry);
       
       for ( std::map<std::vector<int>, TH2D*>::iterator imap=hMap.begin(); imap!=hMap.end(); imap++)
       {
         //cout << "Try to match " << runmin << "," << runmax << " in " << "(" << (*imap).first.at(0) << "," << (*imap).first.at(1)<<")" << endl; 
         if (runmin == unsigned(imap->first.at(0)) && runmax== unsigned(imap->first.at(1)))
         {
           //cout << "Match" << endl;  
           cout << "Before Normalization " << scale_Eop_mean <<  " : " << "normalization" << norm << endl;
           scale_Eop_mean = scale_Eop_mean / norm;   
           cout << "After Normalization " << scale_Eop_mean << endl;
           imap->second ->Scale(1./scale_Eop_mean); 
           break;     
         
         }
         
       }

     }








  
   TFile *outFile = new TFile("testICEBwEop.root","RECREATE");
   for ( std::map<std::vector<int>, TH2D*>::iterator imap=hMap.begin(); imap!=hMap.end(); imap++)
   {

    /*  impa->second()->SetContour(100000);
      impa->second()->SetAxisRange(0.98, 1.02, "Z");

      impa->second() -> SetTitle((to_string(firstRun[i])+"_"+to_string(lastRun[i])).c_str());
      impa->second() -> SetName(("map_ic_"+to_string(i)).c_str());*/
      imap->second -> Write();
  }



   cout << ">>>>>>>> Transferring IC maps with also E/p correction in data/" << outFile->GetName() << endl;

   system("mv testICEBwEop.root /afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/data");


   outFile -> Close();

}
