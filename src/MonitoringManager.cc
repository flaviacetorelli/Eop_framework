#include "MonitoringManager.h"

using namespace std;

MonitoringManager::MonitoringManager(CfgManager conf):
  calibrator(conf),
  conf_(conf),
  h_template_(0)
{
  label_ = conf.GetOpt<string> ("Input.label");  
  variablename_ = conf.GetOpt<string> ("LaserMonitoring.variable");
  SetScaleVariable(variablename_);
  last_accessed_bin_=timebins.end();
}

MonitoringManager::~MonitoringManager()
{
  if(h_template_)
    delete h_template_;
}

void MonitoringManager::SetScaleVariable(const string &variablename)
{
  if(variablename=="ICenergy_over_p")
  {
    cout<<">> SetScaleVariable: special keyword detected"<<endl;
    variabletype_=kICenergy_over_p;
  }
  else
  {
    variabletype_=kregular;
    this -> AddVariable("Eop_monitoring_scale",variablename);//method of ECALELFInterface
  }
}

float MonitoringManager::GetScaleVariableValue(const int &iEle)
{
  if(variabletype_==kICenergy_over_p)
    if(GetP(iEle)!=0)
      return GetICEnergy(iEle)/GetP(iEle);
    else
      return -999;
  else
    return this -> GetVariableValue("Eop_monitoring_scale",iEle);//method of ECALELFInterface
}


TH1F* MonitoringManager::BuildTemplate()
{
  int Nbin   = conf_.GetOpt<int>   ("LaserMonitoring.BuildTemplate.Nbin");
  float xmin = conf_.GetOpt<float> ("LaserMonitoring.BuildTemplate.xmin");
  float xmax = conf_.GetOpt<float> ("LaserMonitoring.BuildTemplate.xmax");

  string templatename = Form("h_template(%i,%f,%f)",Nbin,xmin,xmax);
  
  gDirectory->cd(0);

  cout<<">> Building template \""<<variablename_<<">>"<<templatename<<"\"\n"
      <<"   with selection \""<<selection_str_<<"\"\n"
      <<">> It can take hours.."<<endl; 

  chain_->Draw( Form("%s>>%s",variablename_.c_str(),templatename.c_str()) , selection_str_.c_str() , "goff");
  h_template_=(TH1F*)gDirectory->Get("h_template");
  h_template_->SetName(("h_template_"+label_).c_str());
  h_template_->SetTitle(("h_template_"+label_).c_str());
  //curr_dir->cd();
  return h_template_;
}

void  MonitoringManager::RunDivide()
{
  cout<<">> Running RunDivide"<<endl;
  int    Nevmax_bin     = conf_.GetOpt<int>          ("LaserMonitoring.RunDivide.Nevmax_bin");
  float  maxduration    = 60*60*conf_.GetOpt<float>  ("LaserMonitoring.RunDivide.maxduration");//It is provided in hours

  //Loop on events to build bins corresponding to single lumisections, they will be merged afterward
  //Exploit methods inherited from ECALELFInterface to access the ntuple content
  long Nentries = this->GetEntries();
  cout<<Nentries<<" total entries\n"<<endl;
  for(long ientry=0; ientry<Nentries; ++ientry)
  {
    this->GetEntry(ientry);
    if(ientry%100000==0)
      cout<<"reading entry "<<ientry<<"\r"<<std::flush;
    int w=0;
    if(this->isSelected(0)) ++w;
    if(this->isSelected(1)) ++w;
    if(w)
    {
      unsigned t = this->GetTime();
      auto run =   this->GetRunNumber();
      auto ls =    this->GetLS();
      auto bin_iterator = FindBin(run,ls);
      if(bin_iterator!=timebins.end())
	bin_iterator->AddEvent(run,ls,t);
      else
      {
	TimeBin newbin;
	newbin.SetBinRanges(run,run,ls,ls,t,t);
	newbin.SetNev(w);
	timebins.push_back(newbin);
	last_accessed_bin_ = timebins.end();
      }
    }
  }
  cout<<endl;

  //Merge the lumisections to create TimeBins with about the required number of events
  cout<<">> Merging lumisections"<<endl;
  std::sort(timebins.begin(), timebins.end());
  vector<TimeBin> ls_bins;
  timebins.swap(ls_bins);//switch the contents of timebins vector with the one of ls_bins vector(empty)
  auto lsbin_iterator = ls_bins.begin();
  TimeBin bufferbin(*lsbin_iterator);
  lsbin_iterator++;
  while(lsbin_iterator != ls_bins.end())
  {
    if(bufferbin.DeltaT() > maxduration)
    {
      timebins.push_back(bufferbin);
      bufferbin.Reset();
    }
    else
      if(bufferbin.GetNev() >= Nevmax_bin)
      {
	timebins.push_back(bufferbin);
	bufferbin.Reset();
      }
      else
      {
	bufferbin.AddEvent(*lsbin_iterator);
	lsbin_iterator++;
      }
  }

  if(bufferbin.GetNev() >= Nevmax_bin*0.5)
    timebins.push_back(bufferbin);

  std::sort(timebins.begin(), timebins.end());

}


void  MonitoringManager::SaveTimeBins(std::string outfilename, std::string writemethod)
{
  TFile* outfile = new TFile(outfilename.c_str(), writemethod.c_str());
  outfile->cd();
  TTree* outtree = new TTree(label_.c_str(), label_.c_str());
  TimeBin bin( *(timebins.begin()) );
  bin.BranchOutput(outtree);
  
  for(auto bincontent : timebins)
  {
    bin=bincontent;
    outtree->Fill();
  }

  outfile->cd();
  outtree->AutoSave();
  outfile->Close();
}

void  MonitoringManager::LoadTimeBins(std::string option)
{
  cout<<">> Loading timebins"<<endl; 
  if(timebins.size()>0)
    if(option=="RELOAD")
      timebins.clear();
    else
    {
      cout<<"[WARNING]: timebins not loaded because already in memory"<<endl
	  <<"           if you want to overwrite call LoadTimeBins(\"RELOAD\")"<<endl;
      return;
    }

  string inputfilename = conf_.GetOpt<string>  ("LaserMonitoring.scaleMonitor.runranges");
  cout<<">> Reading file "<<inputfilename<<endl;
  TFile* inputfile = new TFile(inputfilename.c_str(),"READ");
  TTree* intree = (TTree*) inputfile->Get(label_.c_str());
  if(!intree)
  {
    cout<<"[ERROR]: can't get tree "<<label_<<" in the file"<<endl;
    return;
  }
  TimeBin bin;
  bin.BranchInput(intree);
  Long64_t Nbins = intree->GetEntries();
  for(Long64_t ibin=0; ibin<Nbins; ++ibin)
  {
    intree->GetEntry(ibin);//tree entry is copied in bin
    TimeBin bincopy(bin);//perhaps not needed, but for security I avoid to make a mess with pointers
    timebins.push_back(bincopy);
  }
  
  std::sort(timebins.begin(), timebins.end());//It should be already ordered, just for security
  cout<<">> Loaded "<<timebins.size()<<" bins"<<endl;
  inputfile->Close();
  last_accessed_bin_=timebins.end();
}

bool MonitoringManager::BookHistos()
{
  int Nbin_histos = conf_.GetOpt<int>      ("LaserMonitoring.scaleMonitor.Nbin_histos");
  float xmin_histos = conf_.GetOpt<float>  ("LaserMonitoring.scaleMonitor.xmin_histos");
  float xmax_histos = conf_.GetOpt<float>  ("LaserMonitoring.scaleMonitor.xmax_histos");
  for(unsigned ibin=0; ibin<timebins.size(); ++ibin)
    if(!timebins.at(ibin).InitHisto( Form("Histo%i",ibin), Form("Histo%i",ibin), Nbin_histos, xmin_histos, xmax_histos))
      return false;

  return true;
}

//Loop over ECALELF tree to fill the timebins with the corresponding values
void  MonitoringManager::FillTimeBins()
{
  cout<<">> Filling timebin histos with variable "<<variablename_<<endl;
  if(!BookHistos())
  {
    cout<<">> Cannot book the histos... Maybe you have already filled them " << endl;
    return;
  }

  
  long Nentries = this->GetEntries();
  cout<<Nentries<<" total entries\n"<<endl;
  for(long ientry=0; ientry<Nentries; ++ientry)
  {
    this->GetEntry(ientry);
    if(ientry%100000==0)
      cout<<"reading entry "<<ientry<<"\r"<<std::flush;

    for(int iEle=0; iEle<2; ++iEle)
    {
      if(this->isSelected(iEle))
      {
	//cout<<"selected - "<<"run="<<this->GetRunNumber()<<"\tLS="<<this->GetLS()<<"\tT="<<this->GetTime()<<endl;
	auto bin_iterator = FindBin(this->GetRunNumber(),this->GetLS(),this->GetTime());
	if(bin_iterator!=timebins.end())
	  bin_iterator->FillHisto( GetScaleVariableValue(iEle) );
      }
    }
  }

  cout<<">> Histos filled"<<endl;

}

std::vector<TimeBin>::iterator MonitoringManager::FindBin(const UInt_t &run, const UShort_t &ls)
{
  //cout<<"finding bin"<<endl;
  //usually events are in the same time bin of the previous iteration or in adjacent timebins so i start to look for them from there
  std::vector<TimeBin>::iterator it_end   = timebins.end();
  std::vector<TimeBin>::iterator it_begin = timebins.begin();

  if(last_accessed_bin_!=it_end)
  {
    if(last_accessed_bin_ -> Match(run,ls))
      return last_accessed_bin_;

    if(last_accessed_bin_>it_begin)
      if((last_accessed_bin_-1) -> Match(run,ls))
      {
	last_accessed_bin_--;
	return (last_accessed_bin_);
      }

    if(last_accessed_bin_<it_end-1)
       if((last_accessed_bin_+1) -> Match(run,ls))
       {
	 last_accessed_bin_++;
	 return (last_accessed_bin_);
       }
  }    
  //if I am here, unfortunately I have to perform a search through the entire set
  //cout<<"last_accessed_bin_=it_end"<<endl;
  for(std::vector<TimeBin>::iterator it_bin = it_begin; it_bin<it_end; ++it_bin)//not the smarter way considering that the bins are ordered --> can be improved
    if(it_bin -> Match(run,ls))
    {
      //cout<<"match with bin "<<it_bin-it_begin<<endl;
      last_accessed_bin_=it_bin;
      return (last_accessed_bin_);
    }

  //if I am here, I didn't found any match
  return it_end;
}


std::vector<TimeBin>::iterator MonitoringManager::FindBin(const UInt_t &run, const UShort_t &ls, const UInt_t &time)
{
  //cout<<"finding bin"<<endl;
  //usually events are in the same time bin of the previous iteration or in adjacent timebins so i start to look for them from there
  std::vector<TimeBin>::iterator it_end   = timebins.end();
  std::vector<TimeBin>::iterator it_begin = timebins.begin();

  if(last_accessed_bin_!=it_end)
  {
    //cout<<"last_accessed_bin_!=it_end"<<endl;
    if(last_accessed_bin_ -> Match(run,ls,time))
      return last_accessed_bin_;

    if(last_accessed_bin_>it_begin)
      if((last_accessed_bin_-1) -> Match(run,ls,time))
      {
	last_accessed_bin_--;
	return (last_accessed_bin_);
      }

    if(last_accessed_bin_<it_end-1)
       if((last_accessed_bin_+1) -> Match(run,ls,time))
       {
	 last_accessed_bin_++;
	 return (last_accessed_bin_);
       }
  }    
  //if I am here, unfortunately I have to perform a search through the entire set
  //cout<<"last_accessed_bin_=it_end"<<endl;
  for(std::vector<TimeBin>::iterator it_bin = it_begin; it_bin<it_end; ++it_bin)//not the smarter way considering that the bins are ordered --> can be improved
    if(it_bin -> Match(run,ls,time))
    {
      //cout<<"match with bin "<<it_bin-it_begin<<endl;
      last_accessed_bin_=it_bin;
      return (last_accessed_bin_);
    }

  //if I am here, I didn't found any match
  return it_end;
}
	     
	     
		 
	  
void  MonitoringManager::RunComputeMean(string scale)
{
  cout<<">> RunComputeMean in function"<<endl;
  for(std::vector<TimeBin>::iterator it_bin = timebins.begin(); it_bin<timebins.end(); ++it_bin)
  {
    //cout<<"reading bin "<<it_bin-timebins.begin()<<endl;
    it_bin->SetVariable("scale_"+scale, it_bin->GetMean());
  }
}

	  
void  MonitoringManager::RunComputeMedian(string scale)
{
  cout<<">> RunComputeMedian in function"<<endl;
  for(std::vector<TimeBin>::iterator it_bin = timebins.begin(); it_bin<timebins.end(); ++it_bin)
  {
    //cout<<"reading bin "<<it_bin-timebins.begin()<<endl;
    it_bin->SetVariable("scale_"+scale, it_bin->GetMedian());
  }
}

void  MonitoringManager::PrintScales()
{
  auto firstbin=timebins.begin();
  
}


