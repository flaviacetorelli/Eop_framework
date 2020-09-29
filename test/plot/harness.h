#ifndef __HARNESS_H__
#define __HARNESS_H__

#include <iostream>
#include <string>
#include <vector>

using namespace std;
std::vector<std::string> GetHarnessRanges()
{
    std::vector<std::string> HarnessRange;
    //module +- 4,3,2,1
    int ietamin=6;
    int ietamax=25;
    int iphimin=1;
    int iphimax=10;
    for (int keta=0; keta<4; keta++) //loop over eta
    {
        for (int kphi=0; kphi<36; kphi++)
        {
            HarnessRange.push_back(("IEta_" +to_string(ietamin+keta*20)+"_" +to_string(ietamax+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str());
            HarnessRange.push_back(("IEta_-" +to_string(ietamax+keta*20)+"_-" +to_string(ietamin+keta*20)+"_IPhi_"+to_string(iphimin+kphi*10)+"_"+to_string(iphimax+kphi*10)).c_str());
         }

    }
    //module +- 0
    ietamin=1;
    ietamax=5;
    iphimin=1;
    iphimax=20;
    for (int kphi=0; kphi<18; kphi++)
    {

        HarnessRange.push_back(("IEta_" +to_string(ietamin)+"_" +to_string(ietamax)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str());
        HarnessRange.push_back(("IEta_-" +to_string(ietamax)+"_-" +to_string(ietamin)+"_IPhi_"+to_string(iphimin+kphi*20)+"_"+to_string(iphimax+kphi*20)).c_str());
     }

    return HarnessRange;
}

#endif
