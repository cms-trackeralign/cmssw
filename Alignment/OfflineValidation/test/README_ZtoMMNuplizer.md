# ZtoMMNtuplization

Recipe to get it runing:

```
cmsrel CMSSW_13_0_11
cd CMSSW_13_0_11/src
cmsenv
git cms-merge-topic mmusich:ZtoMMNtuplization_13_0_X
scram b -j 20
cd Alignment/OfflineValidation/test
./submitAllScenarios.sh
```

The output files should be created under:

`/eos/cms/store/group/alca_trackeralign/$USER/test_out/layerRotation_studies`