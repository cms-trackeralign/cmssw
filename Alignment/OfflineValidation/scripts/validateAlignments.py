#!/usr/bin/env python3
#test execute: export CMSSW_BASE=/tmp/CMSSW && ./validateAlignments.py -c defaultCRAFTValidation.ini,test.ini -n -N test
from __future__ import print_function
from future.utils import lmap
import subprocess
import json
import yaml
import os
import argparse
import pprint
import sys

import Alignment.OfflineValidation.TkAlAllInOneTool.GCP as GCP
import Alignment.OfflineValidation.TkAlAllInOneTool.DMR as DMR
import Alignment.OfflineValidation.TkAlAllInOneTool.PV as PV
import Alignment.OfflineValidation.TkAlAllInOneTool.SplitV as SplitV
import Alignment.OfflineValidation.TkAlAllInOneTool.JetHT as JetHT

def parser():
    parser = argparse.ArgumentParser(description = "AllInOneTool for validation of the tracker alignment", formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("config", metavar='config', type=str, action="store", help="Global AllInOneTool config (json/yaml format)")
    parser.add_argument("-d", "--dry", action = "store_true", help ="Set up everything, but don't run anything")
    parser.add_argument("-v", "--verbose", action = "store_true", help ="Enable standard output stream")
    parser.add_argument("-e", "--example", action = "store_true", help ="Print example of config in JSON format")
    parser.add_argument("-f", "--force", action = "store_true", help ="Force creation of enviroment, possible overwritten old configuration")
    parser.add_argument("-j", "--job-flavour", action = "store", default = "longlunch", choices = ["espresso", "microcentury", "longlunch", "workday", "tomorrow", "testmatch", "nextweek"], help ="Job flavours for HTCondor at CERN, default is 'longlunch'")

    return parser.parse_args()

def digest_path(path):
    # split path in folders
    path_s = str(path).split(os.sep)

    path_d_s = []
    for part in path_s:
        # Look for environmental variables such as $CMSSW_BASE
        if part.startswith('$'):
            env_var = part[1:].replace('{', '').replace('}', '')
            path_d_s.append(os.environ[env_var])
        else: path_d_s.append(part)

    # re join folders in to a path
    path_d = os.path.join(*path_d_s)

    # re add front / if needed
    if path.startswith(os.sep): path_d = os.sep + path_d

    return path_d


def main():
    ##Read parser arguments
    args = parser()

    ##Print example config which is in Aligment/OfflineValidation/bin if wished
    if args.example:
        with open("{}/src/Alignment/OfflineValidation/bin/example.yaml".format(os.environ["CMSSW_BASE"]), "r") as exampleFile:
            config = yaml.load(exampleFile, Loader=yaml.Loader)
            pprint.pprint(config, width=30)
            sys.exit(0)    

    ##Read in AllInOne config dependent on what format you choose
    with open(args.config, "r") as configFile:
        if args.verbose:
            print("Read AllInOne config: '{}'".format(args.config))

        if args.config.split(".")[-1] == "json":
            config = json.load(configFile)

        elif args.config.split(".")[-1] == "yaml":
            config = yaml.load(configFile, Loader=yaml.Loader)

        else:
            raise Exception("Unknown config extension '{}'. Please use json/yaml format!".format(args.config.split(".")[-1])) 

    ##Digest the LFS path
    if 'LFS' in config: config['LFS'] = digest_path(config['LFS'])
        
    ##Create working directory
    if os.path.isdir(config["name"]) and not args.force:
        raise Exception("Validation directory '{}' already exists! Please choose another name for your directory.".format(config["name"]))	

    validationDir = os.path.abspath(config["name"])
    exeDir = "{}/executables".format(validationDir)
    cmsconfigDir =  "{}/cmsConfigs".format(validationDir)

    subprocess.call(["mkdir", "-p", validationDir] + ((["-v"] if args.verbose else [])))
    subprocess.call(["mkdir", "-p", exeDir] + (["-v"] if args.verbose else []))
    subprocess.call(["mkdir", "-p", cmsconfigDir] + (["-v"] if args.verbose else []))

####################--- Functions ---############################
def createMergeScript( path, validations, options ):
    if(len(validations) == 0):
        raise AllInOneError("Cowardly refusing to merge nothing!")

    repMap = {}

    comparisonLists = {} # directory of lists containing the validations that are comparable
    for validation in validations:
        if validation.config.has_section("IOV"):
            iov = validation.config.get("IOV", "iov")
            validation.defaultReferenceName = iov
        for referenceName in validation.filesToCompare:
            validationtype = type(validation)
            validationName = validation.name
            if validation.config.has_section("IOV") and (referenceName == "Tracker_defaultRange" or referenceName == "Tracker_autoRange"):
                referenceName = iov
            if issubclass(validationtype, PreexistingValidation):
                validationName = validation.originalValName
                #find the actual validationtype
                for parentclass in validationtype.mro():
                    if not issubclass(parentclass, PreexistingValidation):
                        validationtype = parentclass
                        break
            key = (validationtype, validationName, referenceName)


            if key in comparisonLists:
                comparisonLists[key].append(validation)
            else:
                comparisonLists[key] = [validation]
                repMap[key] = validation.config.getGeneral()
                repMap[key].update({
                        "DownloadData":"",
                        "CompareAlignments":"",
                        "RunValidationPlots":"",
                        "CMSSW_BASE": os.environ["CMSSW_BASE"],
                        "SCRAM_ARCH": os.environ["SCRAM_ARCH"],
                        "CMSSW_RELEASE_BASE": os.environ["CMSSW_RELEASE_BASE"],
                        })

                # introduced to merge individual validation outputs separately
                #  -> avoids problems with merge script
                repMap[key]["doMerge"] = "mergeRetCode=0\n"
                repMap[key]["rmUnmerged"] = ("if [[ mergeRetCode -eq 0 ]]; then\n"
                            "    echo -e \\n\"Merging succeeded, removing original files.\"\n")
                repMap[key]["beforeMerge"] = ""
                repMap[key]["mergeParallelFilePrefixes"] = ""
                repMap[key]["createResultsDirectory"]=""

    #print("comparisonLists")
    #pprint.pprint(comparisonLists)
    anythingToMerge = []

    for (validationtype, validationName, referenceName), validations in comparisonLists.items():
        #pprint.pprint("validations")
        #pprint.pprint(validations)
        globalDictionaries.plottingOptions = {}
        lmap( lambda validation: validation.getRepMap(), validations )
        #plotInfo = "plots:offline"
        #allPlotInfo = dict(validations[0].config.items(plotInfo))
        #repMap[(validationtype, validationName, referenceName)].update(allPlotInfo)

        for validation in validations:
            validation.getRepMap()
            #pprint.pprint("validation in validations")
            #pprint.pprint(validation)
            #parallel merging
            if not (isinstance(validation, PreexistingValidation) or validation.NJobs == 1 or not isinstance(validation, ParallelValidation)):
                if (validationtype, validationName, referenceName) not in anythingToMerge:
                    anythingToMerge.append((validationtype, validationName, referenceName))
                    repMap[(validationtype, validationName, referenceName)]["doMerge"] += '\n\n\n\necho -e "\n\nMerging results from %s jobs"\n\n' % validationtype.valType
                    repMap[(validationtype, validationName, referenceName)]["beforeMerge"] += validationtype.doInitMerge()
                repMap[(validationtype, validationName, referenceName)]["doMerge"] += validation.doMerge()
                for f in validation.getRepMap()["outputFiles"]:
                    longName = os.path.join("/eos/cms/store/group/alca_trackeralign/AlignmentValidation/",
                                            validation.getRepMap()["eosdir"], f)
                    repMap[(validationtype, validationName, referenceName)]["rmUnmerged"] += "    rm "+longName+"\n"

        repMap[(validationtype, validationName, referenceName)]["rmUnmerged"] += ("else\n"
                                                                  "    echo -e \\n\"WARNING: Merging failed, unmerged"
                                                                  " files won't be deleted.\\n"
                                                                  "(Ignore this warning if merging was done earlier)\"\n"
                                                                  "fi\n")


        if anythingToMerge:
            repMap[(validationtype, validationName, referenceName)]["DownloadData"] += replaceByMap( configTemplates.mergeParallelResults, repMap[(validationtype, validationName, referenceName)] )
        else:
            repMap[(validationtype, validationName, referenceName)]["DownloadData"] = ""

        repMap[(validationtype, validationName, referenceName)]["RunValidationPlots"] = ""
        repMap[(validationtype, validationName, referenceName)]["plottingscriptpath"] = ""
        if issubclass(validationtype, ValidationWithPlots):
            repMap[(validationtype, validationName, referenceName)]["RunValidationPlots"] = validationtype.doRunPlots(validations)

        repMap[(validationtype, validationName, referenceName)]["CompareAlignments"] = "#run comparisons"
        if issubclass(validationtype, ValidationWithComparison):
            repMap[(validationtype, validationName, referenceName)]["CompareAlignments"] += validationtype.doComparison(validations)

        #if not merging parallel, add code to create results directory and set merge script name accordingly
        if validations[0].config.has_section("IOV"):
            repMap[(validationtype, validationName, referenceName)]["createResultsDirectory"]=replaceByMap(configTemplates.createResultsDirectoryTemplate, repMap[(validationtype, validationName, referenceName)])
            filePath = os.path.join(repMap[(validationtype, validationName, referenceName)]["scriptsdir"], "TkAlMerge.sh")
        else:
            repMap[(validationtype, validationName, referenceName)]["createResultsDirectory"]=replaceByMap(configTemplates.createResultsDirectoryTemplate, repMap[(validationtype, validationName, referenceName)])
            filePath = os.path.join(path, "TkAlMerge.sh")

        theFile = open( filePath, "w" )
        theFile.write( replaceByMap( configTemplates.mergeTemplate, repMap[(validationtype, validationName, referenceName)]) )
        theFile.close()
        os.chmod(filePath,0o755)

def loadTemplates( config ):
    if config.has_section("alternateTemplates"):
        for templateName in config.options("alternateTemplates"):
            if templateName == "AutoAlternates":
                continue
            newTemplateName = config.get("alternateTemplates", templateName )
            #print "replacing default %s template by %s"%( templateName, newTemplateName)
            configTemplates.alternateTemplate(templateName, newTemplateName)

def flatten(l):
    flattenList = []

    for item in l:
        if type(item) == list:
            flattenList.extend(flatten(item))

        else:
            flattenList.append(item)

    return flattenList


####################--- Main ---############################
def main(argv = None):
    if argv == None:
       argv = sys.argv[1:]
    optParser = optparse.OptionParser()
    optParser.description = """All-in-one Alignment Validation.
This will run various validation procedures either on batch queues or interactively.
If no name is given (-N parameter) a name containing time and date is created automatically.
To merge the outcome of all validation procedures run TkAlMerge.sh in your validation's directory.
"""
    optParser.add_option("-n", "--dryRun", dest="dryRun", action="store_true", default=False,
                         help="create all scripts and cfg File but do not start jobs (default=False)")
    optParser.add_option( "--getImages", dest="getImages", action="store_true", default=True,
                         help="get all Images created during the process (default= True)")
    defaultConfig = "TkAlConfig.ini"
    optParser.add_option("-c", "--config", dest="config", default = defaultConfig,
                         help="configuration to use (default TkAlConfig.ini) this can be a comma-seperated list of all .ini file you want to merge", metavar="CONFIG")
    optParser.add_option("-N", "--Name", dest="Name",
                         help="Name of this validation (default: alignmentValidation_DATE_TIME)", metavar="NAME")
    optParser.add_option("-r", "--restrictTo", dest="restrictTo",
                         help="restrict validations to given modes (comma seperated) (default: no restriction)", metavar="RESTRICTTO")
    optParser.add_option("-d", "--debug", dest="debugMode", action="store_true",
                         default = False,
                         help="run the tool to get full traceback of errors",
                         metavar="DEBUG")


    (options, args) = optParser.parse_args(argv)

    if not options.dryRun:
        schedinfo = subprocess.check_output(["myschedd","show"]).decode()
        if not 'tzero' in schedinfo:
            print("\nAll-In-One Tool: you need to call `module load lxbatch/tzero` before trying to submit jobs. Please do so and try again")
            exit(1)


    if not options.restrictTo == None:
        options.restrictTo = options.restrictTo.split(",")

    options.config = [ os.path.abspath( iniFile ) for iniFile in \
                       options.config.split( "," )]

    config = BetterConfigParser()
    outputIniFileSet = set( config.read( options.config ) )
    failedIniFiles = [ iniFile for iniFile in options.config if iniFile not in outputIniFileSet ]

    # Check for missing ini file
    if options.config == [ os.path.abspath( defaultConfig ) ]:
        if ( not os.path.exists( defaultConfig ) ):
                raise AllInOneError( "Default 'ini' file '%s' not found!\n"
                                       "You can specify another name with the "
                                       "command line option '-c'/'--config'."
                                       %( defaultConfig ))
    else:
        for iniFile in failedIniFiles:
            if not os.path.exists( iniFile ):
                raise AllInOneError( "'%s' does not exist. Please check for "
                                       "typos in the filename passed to the "
                                       "'-c'/'--config' option!"
                                       %( iniFile ))
            else:
                raise AllInOneError(( "'%s' does exist, but parsing of the "
                                       "content failed!" ) % iniFile)

    # get the job name
    if options.Name == None:
        existingValDirs = fnmatch.filter( os.walk( '.' ).next()[1],
                                              "alignmentValidation_*" )
        if len( existingValDirs ) > 0:
            options.Name = existingValDirs[-1]
        else:
            print("Cannot guess last working directory!")
            print ( "Please use the parameter '-N' or '--Name' to specify "
                    "the task for which you want a status report." )
            return 1

    # set output path
    outPath = os.path.abspath( options.Name )

    general = config.getGeneral()
    config.set("internals","workdir",os.path.join(general["workdir"],options.Name) )
    config.set("internals","scriptsdir",outPath)
    config.set("general","datadir",os.path.join(general["datadir"],options.Name) )
    config.set("general","logdir",os.path.join(general["logdir"],options.Name) )
    config.set("general","eosdir",os.path.join("AlignmentValidation", general["eosdir"], options.Name) )

    if not os.path.exists( outPath ):
        os.makedirs( outPath )
    elif not os.path.isdir( outPath ):
        raise AllInOneError("the file %s is in the way rename the Job or move it away"%outPath)

    # replace default templates by the ones specified in the "alternateTemplates" section
    loadTemplates( config )

    #save backup configuration file
    backupConfigFile = open( os.path.join( outPath, "usedConfiguration.ini" ) , "w"  )
    config.write( backupConfigFile )

    #copy proxy, if there is one
    try:
        proxyexists = int(getCommandOutput2("voms-proxy-info --timeleft")) > 10
    except RuntimeError:
        proxyexists = False

    if proxyexists:
        shutil.copyfile(getCommandOutput2("voms-proxy-info --path").strip(), os.path.join(outPath, ".user_proxy"))

    validations = []
    jobs = []
    for validation in config.items("validation"):
        validation = validation[0].split("-")
        alignmentList = [validation[1]]
        validationsToAdd = [(validation[0],alignment) \
                                for alignment in alignmentList]
        validations.extend(validationsToAdd)

    for validation in validations:

        job = ValidationJobMultiIOV(validation, config, options, outPath, len(validations))
        if (job.optionMultiIOV == True):
            jobs.extend(job)
        else:
            jobs.extend( ValidationJob(validation, config, options, 1) )

    for job in jobs:
        if job.needsproxy and not proxyexists:
            raise AllInOneError("At least one job needs a grid proxy, please init one.")

    lmap( lambda job: job.createJob(), jobs )

    validations = [ job.getValidation() for job in jobs ]
    validations = flatten(validations)

    createMergeScript(outPath, validations, options)

    lmap( lambda job: job.runJob(), jobs )

    if options.dryRun:
        pass
    else:
        ValidationJobMultiIOV.runCondorJobs(outPath)

    ##Copy AllInOne config in working dir in json/yaml format
    subprocess.call(["cp", "-f", args.config, validationDir] + (["-v"] if args.verbose else []))

    ##List with all jobs
    jobs = []

    ##Check in config for all validation and create jobs
    for validation in config["validations"]:
        if validation == "GCP":
            jobs.extend(GCP.GCP(config, validationDir))

        elif validation == "DMR":
            jobs.extend(DMR.DMR(config, validationDir))

        elif validation == "PV":
            jobs.extend(PV.PV(config, validationDir))

        elif validation == "SplitV":
            jobs.extend(SplitV.SplitV(config, validationDir))

        elif validation == "JetHT":
            jobs.extend(JetHT.JetHT(config, validationDir))

        else:
            raise Exception("Unknown validation method: {}".format(validation)) 
            
    ##Create dir for DAG file and loop over all jobs
    subprocess.call(["mkdir", "-p", "{}/DAG/".format(validationDir)] + (["-v"] if args.verbose else []))

    with open("{}/DAG/dagFile".format(validationDir), "w") as dag:
        for job in jobs:
            ##Create job dir, output dir
            subprocess.call(["mkdir", "-p", job["dir"]] + (["-v"] if args.verbose else []))
            subprocess.call(["mkdir", "-p", job["config"]["output"]] + (["-v"] if args.verbose else []))
            subprocess.call(["ln", "-fs", job["config"]["output"], "{}/output".format(job["dir"])] + (["-v"] if args.verbose else []))

            ##Create symlink for executable/python cms config if needed
            subprocess.call("cp -f $(which {}) {}".format(job["exe"], exeDir) + (" -v" if args.verbose else ""), shell = True)
            subprocess.call(["ln", "-fs", "{}/{}".format(exeDir, job["exe"]), job["dir"]] + (["-v"] if args.verbose else []))
            if "cms-config" in job:
                cmsConfig = job["cms-config"].split("/")[-1]

                subprocess.call(["cp", "-f", job["cms-config"], "{}/{}".format(cmsconfigDir, cmsConfig)] + (["-v"] if args.verbose else []))
                subprocess.call(["ln", "-fs", "{}/{}".format(cmsconfigDir, cmsConfig), "{}/validation_cfg.py".format(job["dir"])] + (["-v"] if args.verbose else []))

            ##Write local config file 
            with open("{}/validation.json".format(job["dir"]), "w") as jsonFile:
                if args.verbose:
                    print("Write local json config: '{}'".format("{}/validation.json".format(job["dir"])))           

                json.dump(job["config"], jsonFile, indent=4)

            ##Write shell executable use in condor job
            with open("{}/run.sh".format(job["dir"]), "w") as runFile:
                if args.verbose:
                    print("Write shell executable: '{}'".format("{}/run.sh".format(job["dir"])))

                runContent = [
                    "#!/bin/bash",
                    "cd $CMSSW_BASE/src",
                    "source /cvmfs/cms.cern.ch/cmsset_default.sh",
                    "eval `scram runtime -sh`",
                    "cd {}".format(job["dir"]),
                    "./{} {}validation.json".format(job["exe"], "validation_cfg.py config=" if "cms-config" in job else ""),
                ]

                # Option the give free arguments to the executable
                if "exeArguments" in job:
                    runContent.pop()
                    runContent.append("./{} {}".format(job["exe"],job["exeArguments"]))

                for line in runContent:
                    runFile.write(line + "\n")

            subprocess.call(["chmod", "a+rx", "{}/run.sh".format(job["dir"])] + (["-v"] if args.verbose else []))

            ##Write condor submit file
            with open("{}/condor.sub".format(job["dir"]), "w") as subFile:
                if args.verbose:
                    print("Write condor submit: '{}'".format("{}/condor.sub".format(job["dir"])))

                subContent = [
                    "universe = vanilla",
                    "getenv = true",
                    "executable = run.sh",
                    "output = condor.out",
                    "error  = condor.err",
                    "log    = condor.log",
                    'requirements = (OpSysAndVer =?= "CentOS7")',
                    # Take given flavour for the job, except if overwritten in job config
                    '+JobFlavour = "{}"'.format(args.job_flavour if not 'flavour' in job else job['flavour']),
                    '+AccountingGroup = "group_u_CMS.CAF.ALCA"',
                    "queue"
                ]

                for line in subContent:
                    subFile.write(line + "\n")

            ##Write command in dag file
            dag.write("JOB {} condor.sub DIR {}\n".format(job["name"], job["dir"]))

            if job["dependencies"]:
                dag.write("\n")
                dag.write("PARENT {} CHILD {}".format(" ".join(job["dependencies"]), job["name"]))

            dag.write("\n\n")

    if args.verbose:
        print("DAGman config has been written: '{}'".format("{}/DAG/dagFile".format(validationDir)))            

    ##Call submit command if not dry run
    if args.dry:
        print("Enviroment is set up. If you want to submit everything, call 'condor_submit_dag {}/DAG/dagFile'".format(validationDir))

    else:
        subprocess.call(["condor_submit_dag", "{}/DAG/dagFile".format(validationDir)])
        
if __name__ == "__main__":
    main()
