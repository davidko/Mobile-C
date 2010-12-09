#!/bin/bash

LOGFILE=`pwd`/testall_output.log

DIRS=`find -type d`

for d in $DIRS; do
  cd $d
  if [ -e client.output ] ; then
    cp ../../testdemo.sh .
    sh ./testdemo.sh >> $LOGFILE
# Wait for bound port to timeout
    sleep 60
    rm testdemo.sh
  fi
  cd -
done
