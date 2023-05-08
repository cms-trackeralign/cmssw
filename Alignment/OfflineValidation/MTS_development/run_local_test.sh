# Parse command line options
plot_only=false
while getopts "p" opt; do
  case ${opt} in
    p)
      plot_only=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

testfiles=""
testfiles=${testfiles}" testfile_MTS_PromptNewTemplate.yaml"
testfiles=${testfiles}" testfile_MTS_mp3619.yaml"

source setup.sh

if [ "$plot_only" = false ]; then
  for testfile in ${testfiles}; do
    cmsRun ${CMSSW_BASE}/src/Alignment/OfflineValidation/python/TkAlAllInOneTool/MTS_cfg.py config=${CMSSW_BASE}/src/Alignment/OfflineValidation/MTS_development/${testfile}
  done
fi

#root -x -b -q TkAlTrackSplitPlot.C++
root -l -b -q TkAlTrackSplitPlot.C++
#validateAlignments.py testfile_MTS.yaml -d
