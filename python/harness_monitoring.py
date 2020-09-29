#!/bin/python
import os
import glob
import math
from array import array
import sys
import time
import subprocess
from optparse import OptionParser
import time
import datetime
import findFiles
import harness_definition

#print date
print("----------------------------------------------------------------------------------")
print(datetime.datetime.now())
print("----------------------------------------------------------------------------------")

#parameters
current_dir = os.getcwd();
ntuple_dir = "/eos/cms/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/ALCARERECO/PromptReco2017_pedv1_ps_ICv1_laserv3_LC_Alpha4/"#parent folder containing all the ntuples of interest
tag_list = ["Run2017B","Run2017C","Run2017D","Run2017E","Run2017F"]#tag for the monitoring = any label in the ntuple path identifying univoquely the ntuples of interest
ignored_ntuples_label_list = ["obsolete","failed"]#ntuples containing anywhere in the path these labels will be ignored (eg corrupted files within the given tag_list)

#parse arguments
parser = OptionParser()
parser.add_option('--submit',          action='store_true',             dest='submit',          default=False,      help='submit jobs')
parser.add_option("-l", "--label",     action="store",      type="str", dest="label",                               help="job label")
parser.add_option("-v", "--verbosity", action="store",      type="int", dest="verbosity",       default=1,          help="verbosity level")
parser.add_option("-o", "--outdir",    action="store",      type="str", dest="outdir",          default="./",       help="output directory")
parser.add_option("-e", "--exedir",    action="store",      type="str", dest="exedir",          default="./bin/",   help="executable directory")
parser.add_option("-c", "--cfg",       action="store",      type="str", dest="configFile",                          help="template config file")
parser.add_option("-t", "--task",      action="store",      type="str", dest="tasklist",        default="runDivide,scaleMonitor",  
                  help="tasks to do it accepts buildTemplate,runDivide,scaleMonitor default:runDivide,scaleMonitor")
parser.add_option('--EE',              action='store_true',             dest='EE',              default=False,      help='run endcap calibration')
parser.add_option('--tier0',           action='store_true',             dest='tier0',           default=False,      help='submit to CAF queues (only if you are logged in lxplus-t0.cern.ch)')

(options, args) = parser.parse_args()

#create outdir
os.system("mkdir -p "+str(options.outdir))

#get ntuples for the calibration
selected_filelist, extracalibtree_filelist = findFiles.findFiles(ntuple_dir,"unmerged",tag_list,ignored_ntuples_label_list)

if (len(selected_filelist)>0):
    print
    print("Run monitoring on "+str(len(selected_filelist))+" files:")
    if(options.verbosity>=1):
        print("-----------------------")
        for filename in selected_filelist:
            print filename 
        print("-----------------------")
        print("auto-generated extraCalibTree filelist")
        for filename in extracalibtree_filelist:
            print filename 
        print("-----------------------")

else:
    print
    print("NOT any file found --> EXIT")
    sys.exit()

#Group the files together
selected_filelist, extracalibtree_filelist = findFiles.groupFilesByTag(selected_filelist, extracalibtree_filelist, tag_list )
if(options.verbosity>=1):
    print "grouped files"
    for filename in selected_filelist:
        print filename 

#create folder for the job
job_parent_folder=current_dir+"/jobs/"+str(options.label)+"/"
os.system("mkdir -p "+job_parent_folder)

#create the log folder
os.system("mkdir -p "+job_parent_folder+"/log/")

#get the harness ranges
harness_ranges = harness_definition.GetHarnessRanges()

#make the monitoring files .cfg, .sh, and .sub
for iFile in range(0,len(selected_filelist)):
    selected_filename=selected_filelist[iFile]
    extracalibtree_filename=extracalibtree_filelist[iFile]
    if(options.verbosity>=1):
        print(">>> Generating job for file "+selected_filename)

    for harness_range in harness_ranges:
        etamin = harness_range[0]
        etamax = harness_range[1]
        phimin = harness_range[2]
        phimax = harness_range[3]

        if(options.verbosity>=1):
            print(">>> Generating job for harness IEta_%i_%i_IPhi_%i_%i"%(etamin,etamax,phimin,phimax))

        jobdir="%s/IEta_%i_%i_IPhi_%i_%i/job_file_%i/"%(job_parent_folder,etamin,etamax,phimin,phimax,iFile)
        os.system("mkdir -p "+jobdir)
        ##### copy executable to the jobDir ######
        os.system('cp '+str(options.exedir)+'LaserMonitoring.exe '+jobdir+'/executable.exe')
        outdir = "%s/IEta_%i_%i_IPhi_%i_%i/"%(options.outdir,etamin,etamax,phimin,phimax) 
        os.system("mkdir -p "+outdir)
        tempdir = "%s/template/IEta_%i_%i_IPhi_%i_%i/file_%i/"%(options.outdir,etamin,etamax,phimin,phimax,iFile) 
        os.system("mkdir -p "+tempdir)

        with open(str(options.configFile)) as fi:
            contents = fi.read()
            replaced_contents = contents.replace("SELECTED_INPUTFILE", selected_filename).replace("EXTRACALIBTREE_INPUTFILE", extracalibtree_filename)
            replaced_contents = replaced_contents.replace("IETAMIN",str(etamin)) 
            replaced_contents = replaced_contents.replace("IETAMAX",str(etamax))
            replaced_contents = replaced_contents.replace("IPHIMIN",str(phimin)) 
            replaced_contents = replaced_contents.replace("IPHIMAX",str(phimax)) 
            replaced_contents = replaced_contents.replace("OUTPUT_RUNDIVIDE","%s/out_file_%i_runranges.root"%(outdir,iFile))
            replaced_contents = replaced_contents.replace("TEMPLATE_FOLDER",tempdir)
            replaced_contents = replaced_contents.replace("OUTPUT_SCALEMONITORING","%s/out_file_%i_scalemonitoring.root"%(outdir,iFile))
            replaced_contents = replaced_contents.replace("OUTPUT_FOLDER",outdir)
            cfgfilename=jobdir+"/config.cfg"
            with open(cfgfilename, "w") as fo:
                fo.write(replaced_contents)

        ##### creates script #######
        outScriptName=jobdir+"/job_file_"+str(iFile)+".sh"
        outScript = open(outScriptName,"w")
        outScript.write("#!/bin/bash\n")
        #outScript.write('source setup.sh\n')
        outScript.write("cd /afs/cern.ch/work/f/fcetorel/private/work2/EFlow/CMSSW_10_5_0/\n")
        outScript.write('eval `scram runtime -sh`\n');
        outScript.write("cd -\n");
        outScript.write("cd "+str(jobdir)+"\n")
        outScript.write("echo 'current dir: ' ${PWD}\n")
        outScript.write("./executable.exe --cfg "+cfgfilename)
        for task in options.tasklist.split(','):
            outScript.write(" --"+task)
        outScript.write("\n")
        outScript.write("echo finish\n") 
        outScript.close();
        os.system("chmod 777 "+outScriptName)


#generate condor multijob submitfile for each task
condorsubFilename=job_parent_folder+"/submit_jobs.sub"
condorsub = open( condorsubFilename,"w")
condorsub.write("executable            = $(scriptname)\n")
condorsub.write("output                = $(scriptname).$(ClusterId).out\n")
condorsub.write("error                 = $(scriptname).$(ClusterId).err\n")
condorsub.write("log                   = "+job_parent_folder+"/log/log.$(ClusterId).log\n")
condorsub.write('+JobFlavour           = "workday"\n')
if options.tier0:
    condorsub.write('+AccountingGroup      = "group_u_CMS.CAF.ALCA"\n')
condorsub.write("queue scriptname matching "+job_parent_folder+"/IEta_*_*_IPhi_*_*/job_file_*/*.sh\n")
condorsub.close()

submit_command = "condor_submit "+condorsubFilename
print("SUBMIT COMMAND: "+submit_command)
#submit in case the option is given
if(options.submit):
    os.system(submit_command)
