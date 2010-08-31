#!/bin/sh
#DEBUG=echo
for team in `ls -d team*`; do
  echo "Sending agents in $team..."
  $DEBUG ./client $team/agent1* bird2.engr.ucdavis.edu 5051
  $DEBUG ./client $team/agent2* phoenix.engr.ucdavis.edu 5052
  $DEBUG ./client $team/agent3* rabbit.engr.ucdavis.edu 5053
  $DEBUG ./client $team/agent4* mouse.engr.ucdavis.edu 5054
#  echo "Done sending agents in $team. continue with next team?(y/n)"
#  read test
#  if [ $test != 'y' ] ; then
#    exit
#  fi
done

echo "Done sending racers."
echo " --- "
echo "Next step is to send the triggers. Send now? (y/n)"

read test

if [ $test != 'y' ] ; then
  exit
fi

for team in `ls -d team*`; do
  $DEBUG ./client $team/trigger* bird2.engr.ucdavis.edu 5051
done
