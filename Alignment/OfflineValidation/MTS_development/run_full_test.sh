# Parse command line options
debug=false
while getopts "d" opt; do
  case ${opt} in
    d)
      debug=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

testfile="testfile_MTS.yaml"

source setup.sh

options=""
if [ "$debug" = true ]; then options=${options}" -d"; fi

validateAlignments.py $(pwd)/${testfile} ${options}
