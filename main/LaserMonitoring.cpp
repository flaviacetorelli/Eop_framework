#include "CfgManager.h"
#include "CfgManagerT.h"
#include "MonitoringManager.h"

#include <iostream>
#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TString.h"


using namespace std;

void PrintUsage()
{
  cout<<"Usage: LaserMonitoring.exe --cfg <cfg_filename> [--buildTemplate] [--runDivide] [--scaleMonitor] [--saveHistos]"<<endl;
}
  
int main(int argc, char* argv[])
{
  string cfgfilename   = "";
  bool   buildTemplate = false;
  bool   runDivide     = false;
  bool   scaleMonitor  = false;
  bool   saveHistos    = false;
  
  //Parse the input options
  for(int iarg=1; iarg<argc; ++iarg)
  {
    if(string(argv[iarg])=="--cfg")
      cfgfilename=argv[iarg+1];
    if(string(argv[iarg])=="--buildTemplate")
      buildTemplate=true;
    if(string(argv[iarg])=="--runDivide")
      runDivide=true;
    if(string(argv[iarg])=="--scaleMonitor")
      scaleMonitor=true;
    if(string(argv[iarg])=="--saveHistos")
      saveHistos=true;
  }
      
  // parse the config file
  if(cfgfilename=="")
  {
    PrintUsage();
    return -1;
  }
  CfgManager config;
  config.ParseConfigFile(cfgfilename.c_str());
  
  // define the monitoring manager object
  MonitoringManager monitor(config);

  // perform the requested tasks
  
  if(buildTemplate)
  {
    TH1F* h_template = monitor.BuildTemplate();
    string outfilename = config.GetOpt<string> ("LaserMonitoring.BuildTemplate.output");
    TFile* outfile = new TFile(outfilename.c_str(),"RECREATE");
    outfile->cd();
    cout<<">> Saving template to "<<outfilename<<"/"<<h_template->GetName()<<endl;
    h_template->Write();
    outfile->Close();
  }

  if(runDivide)
  {
    string outputfilename = config.GetOpt<string> ("LaserMonitoring.RunDivide.output");
    monitor.RunDivide();
    cout<<">> Saving timebins to "<<outputfilename<<endl;
    monitor.SaveTimeBins(outputfilename);
  }

  if(scaleMonitor)
  {
    monitor.LoadTimeBins();

    //fill the histos (one histo per time bin) 
    monitor.FillTimeBins();

    //perform the monitoring, i.e., estrapolate a scale value with the specified method per time bin per variable
    vector<string> MonitoredScales = config.GetOpt<vector<string> > ("LaserMonitoring.scaleMonitor.MonitoredScales");
    for(auto scale : MonitoredScales)
    {
      TString method = config.GetOpt<string> (Form("LaserMonitoring.scaleMonitor.%s.method",scale.c_str()));
      method.ToLower(); //convert capital letters to lower case to avoid mis-understanding
      if(method=="templatefit")
	monitor.RunTemplateFit(scale);
      else
	if(method=="mean")
	  monitor.RunComputeMean(scale);
	else
	  if(method=="median")
	    monitor.RunComputeMedian(scale);
	  else
	  {
	    cout<<"[ERROR]: unknown monitoring method \""<<method<<"\""<<endl;
	    return -1;
	  }
    }
    cout<<"loaded+produced scales are:"<<endl;
    monitor.PrintScales();

    
    //save the output
    string outputfilename = config.GetOpt<string> ("LaserMonitoring.scaleMonitor.output");
    cout<<">> Saving timebins to "<<outputfilename<<endl;
    monitor.SaveTimeBins(outputfilename); // L'unico punto in cui può scambiare è qui! vedi SaveTimeBins in MonitoringManager.cc

    //string writemethod = config.GetOpt<string> ("LaserMonitoring.scaleMonitor.outputmethod");
    //TFile* outfile = new TFile(outfilename.c_str(),writemethod.c_str());
    //monitor.SaveScales(outfile);
    //if(saveHistos)
    //monitor.saveHistos(outfile);
    //outfile->Close();
    
  }//end scaleMonitor
  
  return 0;
  
}
