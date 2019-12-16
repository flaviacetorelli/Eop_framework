#!/usr/bin/env python
import os, re
import commands
import math, time
import sys
import argparse
import subprocess

parser = argparse.ArgumentParser(description='This script splits tasks in multiple jobs and sends them on HTCONDOR')

parser.add_argument("-l", "--label",          required=True,     type=str,  help="job label")
parser.add_argument("-e", "--exe",            required=True,     type=str,  help="executable")
parser.add_argument("-o", "--outputFolder",   required=True,     type=str,  help="folder where to store output files")
#parser.add_argument("-f", "--outputFileName", required=True,     type=str,  help="base name of output files [outputFileName]_i.root")
parser.add_argument("-c", "--configFile",     required=True,     type=str,  help="config file to be run")
#parser.add_argument("-g", "--gpsFile",        required=True,     type=str,  help="gps.mac file to be run")
#parser.add_argument("-n", "--nJobs",          required=True,     type=int,  help="number of jobs")
parser.add_argument("-q", "--queue",          default="workday",     type=str,  help="condor queue to use")
parser.add_argument("-s", "--submit",                                       help="submit jobs", action='store_true')
parser.add_argument("-v", "--verbose",                                      help="increase output verbosity", action='store_true')
parser.add_argument("--htcondor",                                       help="use htcondor", action='store_true')

args = parser.parse_args()


print 
print 'START'
print 

currDir = os.getcwd()

print

try:
   subprocess.check_output(['mkdir','jobs'])
except subprocess.CalledProcessError as e:
   print e.output
try:
   subprocess.check_output(['mkdir','jobs/'+args.label])
except subprocess.CalledProcessError as e:
   print e.output
try:
   subprocess.check_output(['mkdir',args.outputFolder+"/"+args.label+"/"])
except subprocess.CalledProcessError as e:
   print e.output


##### loop for creating and sending jobs #####
'''
#### 1st module
ietamin=1
ietamax=5
iphimin=1
iphimax=20

for keta in range (0,1): #loop over eta
   for kphi in range (0,18):
      #hr="IEta_" +str(ietamin)+"_" +str(ietamax)+"_IPhi_"+str(iphimin+kphi*20)+"_"+str(iphimax+kphi*20)#EB+
      #hr_sel= "(xSeedSC) >= " +str(ietamin)+" && (xSeedSC) <= "+str(ietamax)+" && (ySeedSC) >= "+str(iphimin+kphi*20)+" && (ySeedSC) <= "+str(iphimax+kphi*20)+" && abs(chargeEle)==1 && abs(etaEle) < 1.47" #EB+
      hr_sel= "(xSeedSC) >= -" +str(ietamax)+" && (xSeedSC) <= -"+str(ietamin)+" && (ySeedSC) >= "+str(iphimin+kphi*20)+" && (ySeedSC) <= "+str(iphimax+kphi*20)+" && abs(chargeEle)==1 && abs(etaEle) < 1.47"#EB-
      hr="IEta_-"+str(ietamax)+"_-"+str(ietamin)+"_IPhi_"+str(iphimin+kphi*20)+"_"+str(iphimax+kphi*20)#EB-
'''

#### 2,3,4,5 modules
ietamin=6
ietamax=25
iphimin=1
iphimax=10
for keta in range (0,4): #loop over eta
   for kphi in range (0,36):
      #positive harness IEta>6
      hr="IEta_"+str(ietamin+keta*20)+'_' +str(ietamax+keta*20)+'_IPhi_'+str(iphimin+kphi*10)+'_'+str(iphimax+kphi*10)
      hr_sel= "(xSeedSC) >= " +str(ietamin+keta*20)+" && (xSeedSC) <= "+str(ietamax+keta*20)+" && (ySeedSC) >= "+str(iphimin+kphi*10)+" && (ySeedSC) <= "+str(iphimax+kphi*10)+" && abs(chargeEle)==1 && abs(etaEle) < 1.47"
      #negative harness IEta <-6
      #hr="IEta_-"+str(ietamax+keta*20)+'_-' +str(ietamin+keta*20)+'_IPhi_'+str(iphimin+kphi*10)+'_'+str(iphimax+kphi*10)
      #hr_sel = "(xSeedSC) >= -" +str(ietamax+keta*20)+" && (xSeedSC) <= -"+str(ietamin+keta*20)+" && (ySeedSC) >= "+str(iphimin+kphi*10)+" && (ySeedSC) <= "+str(iphimax+kphi*10)+" && abs(chargeEle)==1 && abs(etaEle) < 1.47"


      ##### creates directory and file list for job #######
      jobDir = currDir+'/jobs/'+args.label+'/job_'+hr

      os.system('mkdir '+jobDir)
      os.chdir(jobDir)
   
      ##### copy executable to the jobDir ######
      os.system('cp '+args.exe+' '+jobDir+"/executable.exe")
   

      
      ##### creates config file #######
      with open(args.configFile) as fi:
         contents = fi.read()
         replaced_contents = contents.replace('SELE_NEW',hr_sel)
         replaced_contents = replaced_contents.replace('OUTPUT_PATH', str(args.outputFolder+args.label+"/scaleMonitor_"+hr+".root"))

      with open(jobDir+"/config.cfg", "w") as fo:
          fo.write(replaced_contents)
   
      ##### creates jobs #######
      with open('job_'+hr+'.sh', 'w') as fout:
         fout.write("#!/bin/sh\n")
         fout.write("echo\n")
         fout.write("echo 'START---------------'\n")
         fout.write("echo 'current dir: ' ${PWD}\n")
         #fout.write("source /afs/cern.ch/sw/lcg/external/gcc/4.9/x86_64-slc6-gcc49-opt/setup.sh\n")
         #fout.write("source /afs/cern.ch/sw/lcg/external/geant4/10.1.p02/x86_64-slc6-gcc49-opt-MT/CMake-setup.sh\n")
         fout.write("mkdir "+str(args.outputFolder+args.label)+"\n")
         fout.write("source /afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/scripts/temporary_setup.sh\n")
         #fout.write("source ~/bin/myRoot.sh\n")
         fout.write("cd "+str(jobDir)+"\n")
         fout.write("echo 'current dir: ' ${PWD}\n")
         fout.write("./executable.exe --cfg config.cfg --scaleMonitor\n")#+args.outputFolder+"/"+args.label+"/"+args.outputFileName+"_"+str(x)+"\n")
         fout.write("mv *.root " +str(args.outputFolder+args.label)+"\n")
         fout.write("echo 'STOP---------------'\n")
         fout.write("echo\n")
         fout.write("echo\n")
      os.system("chmod 755 job_"+hr+".sh")
   
   ###### sends jobs condor ######
      if args.htcondor:
         with open('htcondor_job_'+hr+'.sub', 'w') as fout:
            fout.write("executable            = %s/job_%s.sh\n"%(jobDir,hr))
            fout.write("arguments             = $(ClusterId) $(ProcId)\n")
            fout.write("output                = %s/job_%s.$(ClusterId).$(ProcId).out\n"%(jobDir,hr) )
            fout.write("error                 = %s/job_%s.$(ClusterId).$(ProcId).err\n"%(jobDir,hr) )
            fout.write("log                   = %s/job_%s.$(ClusterId).log\n"%(jobDir,hr) )
            fout.write("transfer_output_files = \"\"\n" )
            fout.write("+JobFlavour           = \"%s\"\n"%(args.queue) )
            fout.write("queue \n")
         if args.submit:
            command = "condor_submit "+jobDir+" htcondor_job_%s.sub"%(hr)
            print command
            os.system(command)
            print "htcondor job harness " + hr + " submitted"

      
      os.chdir("../..")
   
print
print "your jobs:"
os.system("condor_q")
print
print 'END'
print


