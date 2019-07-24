#include "TimeBin.h"

using namespace std;

TimeBin::TimeBin::TimeBin():
  runmin_(0),
  runmax_(0),
  lsmin_(0),
  lsmax_(0),
  timemin_(0),
  timemax_(0),
  Nev_(0),
  h_scale_(0)
  {}

TimeBin::TimeBin::TimeBin(const TimeBin &bincopy):
  runmin_       (bincopy.runmin_),
  runmax_       (bincopy.runmax_),
  lsmin_        (bincopy.lsmin_),
  lsmax_        (bincopy.lsmax_),
  timemin_      (bincopy.timemin_),
  timemax_      (bincopy.timemax_),
  Nev_          (bincopy.Nev_),
  variablelist_ (bincopy.variablelist_)
  {
    if(bincopy.h_scale_)
    {
      h_scale_ = new TH1F(*(bincopy.h_scale_));
      h_scale_->SetDirectory(0);
    }
    else
      h_scale_=0;
  }


TimeBin::TimeBin::~TimeBin()
{
  if(h_scale_)
    delete h_scale_;
}

void TimeBin::TimeBin::SetBinRanges(const UInt_t &runmin, const UInt_t &runmax, const UShort_t &lsmin, const UShort_t &lsmax, const UInt_t &timemin, const UInt_t &timemax)
{
  runmin_=runmin;
  runmax_=runmax;
  lsmin_=lsmin;
  lsmax_=lsmax;
  timemin_=timemin;
  timemax_=timemax;
}

void TimeBin::TimeBin::SetNev(const int &Nev_bin)
{
  Nev_=Nev_bin;
}

TimeBin::TimeBin& TimeBin::TimeBin::operator=(const TimeBin& other)
{
  runmin_       = other.runmin_;
  runmax_       = other.runmax_;
  lsmin_        = other.lsmin_;
  lsmax_        = other.lsmax_;
  timemin_      = other.timemin_;
  timemax_      = other.timemax_;
  Nev_          = other.Nev_;
  variablelist_ = other.variablelist_;
  if(other.h_scale_)
  {
    h_scale_      = new TH1F(*(other.h_scale_));
    h_scale_ -> SetDirectory(0);
  }
  else
    h_scale_=0;
}

bool TimeBin::TimeBin::operator<(const TimeBin& other) const
{
  /*
  cout<<">> In function TimeBin::TimeBin::operator<"<<endl;
  cout<<">> Comparing"<<endl
      <<"(runmin,runmax,lsmin,lsmax,timemin,timemax)=("
      << runmin_ <<","<< runmax_ <<","<< lsmin_ <<","<< lsmax_ <<","<< timemin_ <<","<< timemax_ <<")"<<endl;
  cout<<"with"<<endl
      << other.runmin_ <<","<< other.runmax_ <<","<< other.lsmin_ <<","<< other.lsmax_ <<","<< other.timemin_ <<","<< other.timemax_ <<")"<<endl;
  */
  if(runmin_ < other.runmax_)
    return true;
  else
    if(runmax_ > other.runmin_)
      return false;
    else
      if(lsmin_ < other.lsmax_)
	return true;
      else
	if(lsmax_ > other.lsmin_)
	      return false;
	else
	  if(timemin_ < other.timemax_)
	    return true;
	  else
	    return false;

}

void TimeBin::TimeBin::BranchOutput(TTree* outtree)
{
  outtree->Branch("runmin",&runmin_);
  outtree->Branch("runmax",&runmax_);
  outtree->Branch("lsmin",&lsmin_);
  outtree->Branch("lsmax",&lsmax_);
  outtree->Branch("timemin",&timemin_);
  outtree->Branch("timemax",&timemax_);
  outtree->Branch("Nev",&Nev_);
  for(map<string,float>::iterator it=variablelist_.begin(); it!=variablelist_.end(); ++it)
  {
    string variablename  = it->first;
    float variablevalue = it->second;
    outtree->Branch(variablename.c_str(), &variablevalue);
  }
}

void TimeBin::TimeBin::BranchInput(TTree* intree)
{
  intree->SetBranchAddress("runmin",&runmin_);
  intree->SetBranchAddress("runmax",&runmax_);
  intree->SetBranchAddress("lsmin",&lsmin_);
  intree->SetBranchAddress("lsmax",&lsmax_);
  intree->SetBranchAddress("timemin",&timemin_);
  intree->SetBranchAddress("timemax",&timemax_);
  intree->SetBranchAddress("Nev",&Nev_);
  
  //loop over ttree keys to load all the other variables identified by the prefix "scale_"
  TObjArray *branchList = intree->GetListOfBranches();
  int nBranch = intree->GetNbranches();
  for(int ibranch=0; ibranch<nBranch; ibranch++)
  {
    TString branchname = branchList->At(ibranch)->GetName();
    if(branchname.BeginsWith("scale_"))
    {
      branchname.Remove(0,6);//remove "scale_" prefix from the name
      cout<<"branching "<<branchname.Data()<<endl;
      intree->SetBranchAddress(branchname.Data(),&(variablelist_[branchname.Data()])); 
    }
  }

}


bool TimeBin::TimeBin::Match(const UInt_t &run, const UShort_t &ls, const UInt_t &time) const
{

  if(time>timemin_ && time<timemax_)
    if(run>runmin_ && run<runmax_)
      if(ls>lsmin_ && ls<lsmax_)
	return true;

  return false;

}
	
bool TimeBin::TimeBin::InitHisto( char* name, char* title, const int &Nbin, const double &xmin, const double &xmax)
{
  if(!h_scale_)
  {
    h_scale_=new TH1F(name,title,Nbin,xmin,xmax);
    return true;
  }
  else
    return false;
}