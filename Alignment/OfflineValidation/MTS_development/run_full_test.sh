# Parse command line options
debug=false
verbose=false
compile=false
while getopts "dvc" opt; do
  case ${opt} in
    d)
      debug=true
      ;;
    v)
      verbose=true
      ;;
    c)
      compile=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

testfile="testfile_MTS.yaml"

source setup.sh

if [ "${compile}" = true ]; then ./compile.sh; fi

options=""
if [ "$debug" = true ]; then options=${options}" -d"; fi
if [ "$verbose" = true ]; then options=${options}" -v"; fi
validateAlignments.py $(pwd)/${testfile} ${options}
