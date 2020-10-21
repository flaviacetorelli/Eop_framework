# Eop_framework 
Code for the monitoring and calibration of the ECAL with E/p    
 - algorithm and function are basically a reorganization of ECALELF/EOverPCalibration codes displaced in different branches:      
    - more updated version of calibration code: https://github.com/lbrianza/ECALELF      
    - more updated version of monitoring code:  https://gitlab.cern.ch/vciriolo/ECALELF/tree/eop      
    - master of ECALELF:                        https://gitlab.cern.ch/shervin/ECALELF      
 - this code uses the very useful CfgManager class cloned from https://github.com/simonepigazzini/CfgManager to parse options from file 

For usage, setup, etc.., refer to the [git-wiki](https://github.com/fabio-mon/Eop_framework/wiki)

This particular branch  provides inter calibration IC  with MinBias (Eflow) + Electrons (E/p or Zee).

