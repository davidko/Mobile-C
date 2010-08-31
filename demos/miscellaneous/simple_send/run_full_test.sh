#!/bin/sh

NUM_TRIALS=10

for num_exec in `seq 1 $NUM_TRIALS`; do
  ./mc_server > server_output.txt
  OUTPUT=`grep "total time =" server_output.txt | sort -r`
  TIME=`echo $OUTPUT | sed -e 's/total time = \([0-9]*\.[0-9]*\) .*'
  echo $TIME >> times.txt
done
