//#include "utils.h"
#include "CfgManager.h"
#include "CfgManagerT.h"
#include "calibratorEB.h"
#include "ICmanager.h"

#include <iostream>
#include <string>

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TLorentzVector.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TMath.h"

using namespace std;

void PrintUsage()
{
  cerr << ">>>>> usage:  ComputeIC_EB --cfg <configFileName> --inputIC <objname> <filename> --Eopweight <objtype> <objname> <filename> --ComputeIC_output <outputFileName> --odd[or --even]" << endl;
  cerr << "               " <<            " --cfg                MANDATORY"<<endl;
  cerr << "               " <<            " --inputIC            OPTIONAL, can be also provided in the cfg"<<endl;
  cerr << "               " <<            " --Eopweight          OPTIONAL, can be also provided in the cfg" <<endl;
  cerr << "               " <<            " --ComputeIC_output    OPTIONAL, can be also provided in the cfg" <<endl;
  cerr << "               " <<            " --odd[or --even]     OPTIONAL" <<endl;
}

int main(int argc, char* argv[])
{
  string cfgfilename="";
  vector<string> ICcfg;
  vector<string> weightcfg;
  string outfilename="";
  string splitstat="";

  for(int iarg=1; iarg<argc; ++iarg)
  {
    if(string(argv[iarg])=="--cfg")
      cfgfilename=argv[iarg+1];
    if(string(argv[iarg])=="--inputIC")
    {
      ICcfg.push_back(argv[iarg+1]);
      ICcfg.push_back(argv[iarg+2]);
    }
    if(string(argv[iarg])=="--Eopweight")
    {
      weightcfg.push_back(argv[iarg+1]);
      weightcfg.push_back(argv[iarg+2]);
      weightcfg.push_back(argv[iarg+3]);
    }
    if(string(argv[iarg])=="--ComputeIC_output")
      outfilename=argv[iarg+1];
    if(string(argv[iarg])=="--odd")
      splitstat="odd";
    if(string(argv[iarg])=="--even")
      splitstat="even";
  }

  if(cfgfilename=="")
  {
    PrintUsage();
    return -1;
  }

  // parse the config file
  CfgManager config;
  config.ParseConfigFile(cfgfilename.c_str());
  
  //define the calibrator object to easily access to the ntuples data
  calibratorEB EB(config);

  //set the options directly given as input to the executable, overwriting, in case, the corresponding ones contained in the cfg
  if(weightcfg.size()>0)
    EB.LoadEopWeight(weightcfg);
  if(ICcfg.size()>0)
    EB.LoadIC(ICcfg);

  //eta, phi boundaries are taken by the calibrator constructor from configfile 
  float ietamin, ietamax, iphimin, iphimax;
  int Neta, Nphi;
  EB.GetEtaboundaries(ietamin, ietamax);
  EB.GetPhiboundaries(iphimin, iphimax);
  Neta=EB.GetNeta();
  Nphi=EB.GetNphi();

  //define the output 
  if(outfilename == "")
    if(config.OptExist("Output.ComputeIC_output"))
      outfilename = config.GetOpt<string> ("Output.ComputeIC_output");
    else
      outfilename = "IC.root";

  TFile *outFile = new TFile(outfilename.c_str(),"RECREATE");
  ICmanager numerator(ietamin,ietamax,iphimin,iphimax);
  ICmanager denominator(ietamin,ietamax,iphimin,iphimax);

  //Initialize numerator and denominator
  numerator.InitIC(0.);
  denominator.InitIC(0.);

  //loop over entries to fill the histo  
  Long64_t Nentries=EB.GetEntries();
  cout<<Nentries<<" entries"<<endl;
  if(Nentries==0)
    return -1;
  float E,p,eta,IC,weight,regression;
  int iEle, index, ieta, iphi, ietaSeed;
  vector<float>* ERecHit;
  vector<float>* fracRecHit;
  vector<int>*   XRecHit;
  vector<int>*   YRecHit;
  vector<int>*   ZRecHit;
  vector<int>*   recoFlagRecHit;


  //set the iteration start and the increment accordingly to splitstat
  int ientry0=0;
  int ientry_increment=1;
  if(splitstat=="odd")
  {
    ientry0=1;
    ientry_increment=2;
  }
  else
    if(splitstat=="even")
    {
      ientry0=0;
      ientry_increment=2;
    }
  
  for(Long64_t ientry=ientry0 ; ientry<Nentries ; ientry+=ientry_increment)
  {
    if( ientry%100000==0 || (ientry-1)%100000==0)
      std::cout << "Processing entry "<< ientry << "\r" << std::flush;
    EB.GetEntry(ientry);
    for(iEle=0;iEle<2;++iEle)
    {
      if(EB.isSelected(iEle))
      {
	ERecHit=EB.GetERecHit(iEle);
	fracRecHit=EB.GetfracRecHit(iEle);
	XRecHit=EB.GetXRecHit(iEle);
	YRecHit=EB.GetYRecHit(iEle);
	//ZRecHit=EB.GetZRecHit(iEle);
	recoFlagRecHit=EB.GetrecoFlagRecHit(iEle);
	E=EB.GetICEnergy(iEle);
	p=EB.GetPcorrected(iEle);
	//eta=EB.GetEtaSC(iEle);
	ietaSeed=EB.GetietaSeed(iEle);
	weight=EB.GetWeight(ietaSeed,E/p);
	regression=EB.GetRegression(iEle);
	//cout<<"E="<<E<<"\tp="<<p<<"\teta="<<eta<<"\tweight="<<weight<<"\tregression="<<regression<<endl;
	for(unsigned iRecHit=0; iRecHit<ERecHit->size(); ++iRecHit)
	{
	  if(recoFlagRecHit->at(iRecHit) >= 4)
	    continue;
	  ieta=XRecHit->at(iRecHit);
	  iphi=YRecHit->at(iRecHit);
	  IC=EB.GetIC(ieta,iphi);
	  //cout<<"entry="<<ientry<<endl;
	  //cout<<"ieta="<<ieta<<"\tiphi="<<iphi<<"\tindex="<<index<<"\tIC="<<IC<<endl;
	  //cout<<"ERH="<<ERecHit->at(iRecHit)<<"\tfracRH="<<fracRecHit->at(iRecHit)<<endl;
	  //cout<<"regression="<<regression<<"\tE="<<E<<"\tp="<<p<<"\tweight="<<weight<<endl;
	  //if(E>15. && p>15.)
	  {
	    numerator(ieta,iphi)   += ERecHit->at(iRecHit) * fracRecHit->at(iRecHit) * regression * IC / E * p / E * weight;
	    denominator(ieta,iphi) += ERecHit->at(iRecHit) * fracRecHit->at(iRecHit) * regression * IC / E         * weight;
	    //cout<<"numerator="<<numerator[index]<<"\tdenominator="<<denominator[index]<<endl;
	    //if(ieta>70) getchar();
	  }

	  //else
	  //  cout<<"[WARNING]: E="<<E<<" and p="<<p<<" for event "<<ientry<<endl;
	}
      }
    }
  }	  

  //get numerator and denominator histos
  TH2D* h2_numerator = numerator.GetHisto("numerator","numerator");
  TH2D* h2_denominator = denominator.GetHisto("denominator","denominator");

  //compute temporary IC-pull and IC-values 
  TH2D* h2_ICpull = GetICpull(h2_numerator,h2_denominator);
  TH2D* h2_temporaryIC = EB.GetPulledIC(h2_ICpull);

  h2_temporaryIC->SetName("temporaryIC");
  h2_temporaryIC->SetTitle("temporaryIC");

  //save and close
  outFile->cd();
  h2_numerator->Write();
  h2_denominator->Write();
  h2_ICpull->Write();
  h2_temporaryIC->Write();
  outFile->Close();
  return 0;
}
