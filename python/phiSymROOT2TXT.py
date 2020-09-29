# import ROOT in batch mode
from __future__ import division
import sys
import os
import re
import argparse
import math
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
import ROOT
ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

#from PhiSym.EcalCalibAlgos.EcalCalibAnalysis import *

parser = argparse.ArgumentParser (description = 'Dump EFlow + E/p IC correction from a ROOT file')
parser.add_argument('inputfile' , default="../data/testICEBwEop.root", help='analyze this file')
parser.add_argument('outputdir' , default="", help='output directory')

opts = parser.parse_args ()

#/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/
# Get the crystals trees
inFile = ROOT.TFile(opts.inputfile)
outdir = opts.outputdir




#
for imap in range(0,100):
    print ">>> reading histogram ", imap 
    ICmap = inFile.Get("map_ic_"+str(imap))
    run = ICmap.GetTitle()
    os.system("mkdir -p "+outdir+run)
    outFile = open (outdir+run+"/file.txt", "w")

#EB
    Neta_=ICmap.GetNbinsY();
    ietamin_=ICmap.GetYaxis().GetXmin()
    ietamax_=ICmap.GetYaxis().GetXmax()-1 #i want the left limit of last bin, not the right one
    Nphi_=ICmap.GetNbinsX()
    iphimin_=ICmap.GetXaxis().GetXmin();
    iphimax_=ICmap.GetXaxis().GetXmax()-1 #i want the left limit of last bin, not the right one
    #cout<<">>> Neta="<<Neta_<<" in ["<<ietamin_<<","<<ietamax_<<"] and Nphi="<<Nphi_<<" in ["<<iphimin_<<","<<iphimax_<<"]"<<endl;
    xbin=1
    ybin=1
    #print "f"
    #print "# status = used/unused (1/0) in ring sumEt avarage computation, or bad channel (-1)"
    #outFile.write ("# ieta(ix) -- iphi(iy) -- zside -- IC -- IC_err"+ "\n")
    for ybin in range (1,Neta_+1) :
        if ICmap.GetYaxis().GetBinLowEdge(ybin) == 0: continue
        for xbin in range (1,Nphi_+1) :

            ebIC = ICmap.GetBinContent(xbin,ybin)
            ieta = ICmap.GetYaxis().GetBinLowEdge(ybin)
            iphi = ICmap.GetXaxis().GetBinLowEdge(xbin)
            
            # print "ieta " ,ieta , "iphi " , iphi,  ebIC, "ebIC"
    
            
            if ebIC <= 0:
                ebIC = 1.
            #outFile.write( int(ieta), int(iphi), 0, "%.5f" % ebIC, 1.)
            outFile.write( str(int(ieta)) + " "  )
            outFile.write( str(int(iphi)) + " "  )
            outFile.write( str(0) + " "  )
            outFile.write( str(ebIC) + " "  )
            outFile.write( str(0.) + "\n"  )


    outFile.close()

