#!/usr/bin/env python
import os, re
import commands
import math, time
import sys
import argparse
import subprocess

parser = argparse.ArgumentParser(description='This script splits Geant4 tasks in multiple jobs and sends them on LXBATCH')

parser.add_argument("-l", "--label",          required=True,     type=str,  help="job label")
parser.add_argument("-e", "--exe",            required=True,     type=str,  help="executable")
parser.add_argument("-o", "--outputFolder",   required=True,     type=str,  help="folder where to store output files")
#parser.add_argument("-f", "--outputFileName", required=True,     type=str,  help="base name of output files [outputFileName]_i.root")
parser.add_argument("-c", "--configFile",     required=True,     type=str,  help="config file to be run")
#parser.add_argument("-g", "--gpsFile",        required=True,     type=str,  help="gps.mac file to be run")
parser.add_argument("-n", "--nJobs",          required=True,     type=int,  help="number of jobs")
#parser.add_argument("-N", "--nEvents",        required=True,     type=int,  help="number of events per job")
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
runranges = ["'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297175_297178.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297178_297219.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297219_297219.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297219_297227.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297292_297296.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297296_297359.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297411_297431.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297431_297483.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297483_297487.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297487_297505.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297505_297563.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297563_297605.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297605_297656.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_297665_297723.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_298996_299065.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_299065_299149.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_299178_299185.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_299368_299420.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_299443_299481.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_299481_299594.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_299594_299597.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_299649_299649.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300087_300123.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300123_300157.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300157_300237.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300237_300240.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300280_300284.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300364_300401.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300459_300467.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300467_300517.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300558_300576.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300576_300636.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300636_300742.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300777_300785.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_300785_300817.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301046_301046.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301141_301183.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301281_301323.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301323_301392.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301392_301417.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301447_301461.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301472_301519.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301519_301567.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301627_301665.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301694_301694.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301912_301941.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301959_301985.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_301985_301998.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302031_302043.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302131_302166.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302225_302240.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302262_302277.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302279_302328.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302337_302393.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302393_302448.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302472_302485.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302485_302525.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302525_302573.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302573_302597.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_302597_302597.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_303825_303838.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_303838_303885.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_303885_303998.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_303998_304062.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304062_304144.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304144_304158.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304158_304169.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304169_304209.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304209_304292.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304333_304366.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304366_304447.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304447_304508.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304562_304626.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304654_304671.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304671_304738.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_304738_304778.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305044_305064.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305064_305081.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305112_305186.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305188_305204.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305207_305237.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305247_305311.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305311_305351.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305358_305377.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305377_305406.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305406_305516.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305516_305589.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305589_305636.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305636_305636.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305766_305809.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305814_305821.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305832_305842.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_305862_305902.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_306029_306051.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_306091_306125.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_306125_306135.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_306135_306139.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_306153_306170.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_306170_306171.root'",
"'/afs/cern.ch/work/f/fcetorel/private/work2/Eop/Eop_framework/test/runranges_306418_306459.root'"]

map_ic = ["'map_ic_297175_297178'",
"'map_ic_297178_297219'",
"'map_ic_297219_297219'",
"'map_ic_297219_297227'",
"'map_ic_297292_297296'",
"'map_ic_297296_297359'",
"'map_ic_297411_297431'",
"'map_ic_297431_297483'",
"'map_ic_297483_297487'",
"'map_ic_297487_297505'",
"'map_ic_297505_297563'",
"'map_ic_297563_297605'",
"'map_ic_297605_297656'",
"'map_ic_297665_297723'",
"'map_ic_298996_299065'",
"'map_ic_299065_299149'",
"'map_ic_299178_299185'",
"'map_ic_299368_299420'",
"'map_ic_299443_299481'",
"'map_ic_299481_299594'",
"'map_ic_299594_299597'",
"'map_ic_299649_299649'",
"'map_ic_300087_300123'",
"'map_ic_300123_300157'",
"'map_ic_300157_300237'",
"'map_ic_300237_300240'",
"'map_ic_300280_300284'",
"'map_ic_300364_300401'",
"'map_ic_300459_300467'",
"'map_ic_300467_300517'",
"'map_ic_300558_300576'",
"'map_ic_300576_300636'",
"'map_ic_300636_300742'",
"'map_ic_300777_300785'",
"'map_ic_300785_300817'",
"'map_ic_301046_301046'",
"'map_ic_301141_301183'",
"'map_ic_301281_301323'",
"'map_ic_301323_301392'",
"'map_ic_301392_301417'",
"'map_ic_301447_301461'",
"'map_ic_301472_301519'",
"'map_ic_301519_301567'",
"'map_ic_301627_301665'",
"'map_ic_301694_301694'",
"'map_ic_301912_301941'",
"'map_ic_301959_301985'",
"'map_ic_301985_301998'",
"'map_ic_302031_302043'",
"'map_ic_302131_302166'",
"'map_ic_302225_302240'",
"'map_ic_302262_302277'",
"'map_ic_302279_302328'",
"'map_ic_302337_302393'",
"'map_ic_302393_302448'",
"'map_ic_302472_302485'",
"'map_ic_302485_302525'",
"'map_ic_302525_302573'",
"'map_ic_302573_302597'",
"'map_ic_302597_302597'",
"'map_ic_303825_303838'",
"'map_ic_303838_303885'",
"'map_ic_303885_303998'",
"'map_ic_303998_304062'",
"'map_ic_304062_304144'",
"'map_ic_304144_304158'",
"'map_ic_304158_304169'",
"'map_ic_304169_304209'",
"'map_ic_304209_304292'",
"'map_ic_304333_304366'",
"'map_ic_304366_304447'",
"'map_ic_304447_304508'",
"'map_ic_304562_304626'",
"'map_ic_304654_304671'",
"'map_ic_304671_304738'",
"'map_ic_304738_304778'",
"'map_ic_305044_305064'",
"'map_ic_305064_305081'",
"'map_ic_305112_305186'",
"'map_ic_305188_305204'",
"'map_ic_305207_305237'",
"'map_ic_305247_305311'",
"'map_ic_305311_305351'",
"'map_ic_305358_305377'",
"'map_ic_305377_305406'",
"'map_ic_305406_305516'",
"'map_ic_305516_305589'",
"'map_ic_305589_305636'",
"'map_ic_305636_305636'",
"'map_ic_305766_305809'",
"'map_ic_305814_305821'",
"'map_ic_305832_305842'",
"'map_ic_305862_305902'",
"'map_ic_306029_306051'",
"'map_ic_306091_306125'",
"'map_ic_306125_306135'",
"'map_ic_306135_306139'",
"'map_ic_306153_306170'",
"'map_ic_306170_306171'",
"'map_ic_306418_306459'"]




##### loop for creating and sending jobs #####
for x in range(1, len(map_ic)):

   ##### creates directory and file list for job #######
   jobDir = currDir+'/jobs/'+args.label+'/job_'+str(x)
   os.system('mkdir '+jobDir)
   os.chdir(jobDir)
   
   ##### copy executable to the jobDir ######
   os.system('cp '+args.exe+' '+jobDir+"/executable.exe")
   
   ##### creates Geant4 gps.mac file #######
   #with open(args.gpsFile) as fi:
   #   contents = fi.read()
   #   replaced_contents = contents.replace('MAXEVENTS', str(args.nEvents))
   #with open(jobDir+"/gps.mac", "w") as fo:
   #   fo.write(replaced_contents)
      
   ##### creates Geant4 config file #######
   with open(args.configFile) as fi:
      contents = fi.read()
      replaced_contents = contents.replace('MAP',map_ic[x-1])
      #replaced_contents = replaced_contents.replace('GPS', jobDir+"/gps.mac")
      replaced_contents = replaced_contents.replace('RUN', runranges[x-1])
      replaced_contents = replaced_contents.replace('OUTPUT_PATH', str(args.outputFolder+args.label+"/scaleMonitor_"+str(x)+".root"))
      #replaced_contents = replaced_contents.replace('/afs/cern.ch/user/f/fmonti', '~fmonti') #problems between geant4 and the absolute path, in particular cern.ch is automatically modified in cern_t0.ch by g4analysismanager
   with open(jobDir+"/config.cfg", "w") as fo:
      fo.write(replaced_contents)
   
   ##### creates jobs #######
   with open('job_'+str(x)+'.sh', 'w') as fout:
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
   os.system("chmod 755 job_"+str(x)+".sh")
   
   ###### sends bjobs ######

   if args.htcondor:
      with open('htcondor_job_'+str(x)+'.sub', 'w') as fout:
         fout.write("executable            = %s/job_%s.sh\n"%(jobDir,str(x)))
         fout.write("arguments             = $(ClusterId) $(ProcId)\n")
         fout.write("output                = %s/job_%s.$(ClusterId).$(ProcId).out\n"%(jobDir,str(x)) )
         fout.write("error                 = %s/job_%s.$(ClusterId).$(ProcId).err\n"%(jobDir,str(x)) )
         fout.write("log                   = %s/job_%s.$(ClusterId).log\n"%(jobDir,str(x)) )
         fout.write("transfer_output_files = \"\"\n" )
         fout.write("+JobFlavour           = \"%s\"\n"%(args.queue) )
         fout.write("queue \n")
      if args.submit:
         command = "condor_submit "+jobDir+" htcondor_job_%s.sub"%(str(x))
         print command
         os.system(command)
         print "htcondor job nr. " + str(x) + " submitted"

      
   os.chdir("../..")
   
print
print "your jobs:"
os.system("condor_q")
print
print 'END'
print


