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
    TString method = argv[1];
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
     TFile* inEopfile;
     //Apro il file di E/p su tutto il Barrel per correggere la scala 
    if (method == "mean")
    {
    inEopfile = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/barrel/", "READ");
    }

    else if (method == "templatefit")
    {
    inEopfile = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/barrel_template_EflowCorr/IEta_-85_85_IPhi_1_360/out_file_scalemonitoring.root", "READ");
    }

    else 
    {
    cout << "No acceptable method, try ./SaveICmap_EopCorr [method=mean, templatefit]" << endl;
    return 0;
    }
    unsigned runmin, runmax; 
    float scale_Eop, scaleunc_Eop; 
    TTree *t= (TTree*)inEopfile->Get("ciao"); 
    t->SetBranchAddress("runmin",&runmin); 
    t->SetBranchAddress("runmax", &runmax); 
    t->SetBranchAddress("scale_Eop_"+method, &scale_Eop); 
    t->SetBranchAddress("scaleunc_Eop_"+method, &scaleunc_Eop); 

    float norm, mean, emean; 
  /*  for (int i= 0; i<5; i++) // normalization to the w mean of first 5 points
    {
       t->GetEntry(i);
       mean += scale_Eop*(1./scaleunc_Eop)*(1./scaleunc_Eop);
       emean += (1./scaleunc_Eop)*(1./scaleunc_Eop);
       //cout << "This is mean" << mean << endl; 
    }
    norm = mean/emean;*/
   // cout << "First point for normalization "<< norm << endl; 

    t -> GetEntry(0);
    norm = scale_Eop;
    for (long ientry=0; ientry < t->GetEntries(); ientry++)
    {
       t->GetEntry(ientry);

       cout << norm << endl;
       cout << runmin << endl;
       cout << runmax << endl;
       
       for ( std::map<std::vector<int>, TH2D*>::iterator imap=hMap.begin(); imap!=hMap.end(); imap++)
       {
         //cout << "Try to match " << runmin << "," << runmax << " in " << "(" << (*imap).first.at(0) << "," << (*imap).first.at(1)<<")" << endl; 
         if (runmin == unsigned(imap->first.at(0)) && runmax== unsigned(imap->first.at(1)))
         {
           //cout << "Match" << endl;  
           cout << "Before Normalization " << scale_Eop <<  " : " << "normalization" << norm << endl;
           scale_Eop = scale_Eop / norm;   
           cout << "After Normalization " << scale_Eop << endl;
           imap->second ->Scale(1./scale_Eop); 
           break;     
         
         }
         
       }

     }


  
   TFile *outFile = new TFile("/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/data/ICEBwEopNorm_"+method+".root","RECREATE");
   for ( std::map<std::vector<int>, TH2D*>::iterator imap=hMap.begin(); imap!=hMap.end(); imap++)
   {
      imap->second -> Write();
  }



   cout << ">>>>>>>> Transferring IC maps with also E/p correction in data/" << outFile->GetName() << endl;


   outFile -> Close();

}
