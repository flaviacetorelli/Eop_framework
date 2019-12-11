#ifndef FITUTILS__
#define FITUTILS__

#include <iostream>
#include <string>

#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"

using namespace std;

namespace FitUtils
{
  
  bool PerseverantFit( TH1* h, TF1* fitfunc, int nTrial=10, string TemplatePlotsFolder="")
  {
    TFitResultPtr rp;
    int fStatus;
    TCanvas c_template_fit;//canvas to store the result of the template fit if requested by cfg
    std::cout << "Histogram " << h->GetName() << std::endl; //debug
//#ifdef DEBUG
    std::cout << "entries: " << h->GetEntries() << std::endl;//debug
//#endif
    if (h->GetEntries() == 0) 
    {  
      return false; 
      if(TemplatePlotsFolder!="")
	  c_template_fit.Print( Form("%s/fit_%s.png",TemplatePlotsFolder.c_str(), h->GetName()) );
    }
    fitfunc -> SetParameter(1, 0.99);

    for(int nTrial = 0; nTrial < 10; ++nTrial)
    {
      c_template_fit.cd();
      rp = h -> Fit(fitfunc, "QRL+");
      fStatus = rp;
      std::cout << "# of trial: " << nTrial << std::endl; //debug
      std::cout << "fit Status: " << fStatus; //debug
      std::cout << " && parameter error: " << fitfunc->GetParError(1) << std::endl;//debug
      if(fStatus != 4 && fitfunc->GetParError(1) != 0.)
      {
	if(TemplatePlotsFolder!="")
	  c_template_fit.Print( Form("%s/fit_%s.png",TemplatePlotsFolder.c_str(), h->GetName()) );
        std::cout << "So I'm True " << std::endl; //debug
	return true;
      }
    }
    if(TemplatePlotsFolder!="")
      c_template_fit.Print( Form("%s/fit_%s.png",TemplatePlotsFolder.c_str(), h->GetName()) );
    std::cout << "So I'm False" << std::endl; //debug
    return false;
  }

}

#endif
