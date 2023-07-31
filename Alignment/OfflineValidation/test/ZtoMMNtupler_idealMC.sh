#!/bin/bash

myScenario=$1
JobName=ZtoMuMuNtuplization_$myScenario

echo  "Job started at " `date`

CMSSW_DIR=$2
LXBATCH_DIR=$PWD

cd ${CMSSW_DIR}
eval `scramv1 runtime -sh`
cd $LXBATCH_DIR

cp ${CMSSW_DIR}/ZtoMMNtupler_idealMC_WithRefit_cfg.py .
cp ${CMSSW_DIR}/listOfFiles_idealMC_TkAlDiMuonAndVertex.txt .
echo "cmsRun ${CMSSW_DIR}/ZtoMMNtupler_idealMC_WithRefit_cfg.py scenario=${myScenario}"

cmsRun ZtoMMNtupler_idealMC_WithRefit_cfg.py scenario=${myScenario} >& ${JobName}.out

echo "Content of working directory is: " `ls -lrt`

eos mkdir -p /eos/cms/store/group/alca_trackeralign/$USER/test_out/layerRotation_studies

for payloadOutput in $(ls *root ); do xrdcp -f $payloadOutput root://eoscms.cern.ch//eos/cms/store/group/alca_trackeralign/$USER/test_out/layerRotation_studies; done

mv ${JobName}.out ${CMSSW_DIR}/outfiles
mv ${JobName}.err ${CMSSW_DIR}/outfiles
mv ${JobName}.log ${CMSSW_DIR}/outfiles

echo  "Job ended at " `date`

exit 0
