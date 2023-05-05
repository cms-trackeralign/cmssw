#!/usr/bin/env bash

CURRENT_DIR=$(pwd)

SOURCE=${BASH_SOURCE[0]}
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
  SOURCE=$(readlink "$SOURCE")
  [[ $SOURCE != /* ]] && SOURCE=$DIR/$SOURCE # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )

export WORK_BASE_AREA=$(echo $DIR | rev | cut -c 17- | rev)

cd ${DIR}

cmsenv
if [ ! $(voms-proxy-info --actimeleft 2>/dev/null) ]; then
  voms-proxy-init --rfc --voms cms -valid 192:00
fi
cd ${CURRENT_DIR}

echo "Environment for tracker alignment sourced correctly"
return 0
