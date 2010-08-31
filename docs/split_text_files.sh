#!/bin/sh

# This script is meant to split text files that are too long for
# the \verbatiminput{} in latex.

FILELIST=`find ../demos/ -name '*.xml'`
SPLITLEN=50


TAILLEN=`expr $SPLITLEN + 1`

for i in $FILELIST; do
  LINENUM=`wc -l $i | awk '{print $1}'`
  if [ "$LINENUM" -ge 55 ] ; then
    echo "File $i is Long! Converting..."
    head -n $SPLITLEN $i > $i.part1
    tail -n +$TAILLEN $i > $i.part2
    echo "Done."
  fi
done

