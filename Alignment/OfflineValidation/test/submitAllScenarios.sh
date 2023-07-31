#!/bin/bash
eval `scramv1 runtime -sh`
CMSSW_DIR=${CMSSW_BASE}/src/Alignment/OfflineValidation/test

if [ -d $CMSSW_DIR/outfiles ]; then
    echo "$CMSSW_DIR/outfiles already exists, skipping"
else
    mkdir $CMSSW_DIR/outfiles
fi

# Define an array
scenarios=('-10e-6' '-8e-6' '-6e-6' '-4e-6' '-2e-6' '0' '2e-6' '4e-6' '6e-6' '8e-6' '10e-6')

# Loop over the array and print each element
for scenario in "${scenarios[@]}"
do
  echo "$scenario"
  condor_submit par1=${scenario} par2=${CMSSW_DIR} submit.sub
done
