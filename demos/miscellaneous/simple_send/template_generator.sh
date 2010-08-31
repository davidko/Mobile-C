#!/bin/sh

#DEBUG=echo

echo "This script takes 2 arguments. The first is the number of agents per team,"
echo "the second is the number of teams."

echo "Continue? (y/n)"

read test

if [ $test != 'y' ] ; then
  exit
fi

numhosts=`wc host_list.txt | awk ' { print $1 } '`
for team in `seq 1 $2`; do
  $DEBUG mkdir team$team
  let cond1=100+$team
  let cond2=1000+$team
  agent=1
  agent_filename="agent"$agent"_team"$team".xml"
  $DEBUG cp template_racer_init.xml team$team/$agent_filename
  # Here, we use sed to replace some tags
  $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_AGENT_NAME/"agent"$agent"_team"$team/g"
  $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_COND1/$cond1/g"
  $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_COND2/$cond2/g"
  let servercount=0
  for server in `cat host_list.txt`; do
    let servernum=201-$agent
    let servernum=$servernum%4+$servercount
    let servernum=$servernum%4
    let servernum=$servernum+1
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_SERVER$servernum/$server/g"
    let servercount=$servercount+1
  done
  for agent in `seq 2 $1`; do
    agent_filename="agent"$agent"_team"$team".xml"
    $DEBUG cp template_racer.xml team$team/$agent_filename
    # Here, we use sed to replace some tags
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_AGENT_NAME/"agent"$agent"_team"$team/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_COND1/$cond1/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_COND2/$cond2/g"
    let servercount=0
    for server in `cat host_list.txt`; do
      let servernum=201-$agent
      let servernum=$servernum%4+$servercount
      let servernum=$servernum%4
      let servernum=$servernum+1
      $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_SERVER$servernum/$server/g"
      let servercount=$servercount+1
    done
  done
  # Now do the trigger
  trigger_filename=team$team/trigger_team$team.xml
  $DEBUG cp template_trigger.xml $trigger_filename
  $DEBUG sed $trigger_filename -i -e "s/TEMPLATE_AGENT_NAME/trigger_team$team/g"
  $DEBUG sed $trigger_filename -i -e "s/TEMPLATE_COND1/$cond1/g"
  $DEBUG sed $trigger_filename -i -e "s/TEMPLATE_COND2/$cond2/g"
  let servercount=1
  for server in `cat host_list.txt`; do
    $DEBUG sed $trigger_filename -i -e "s/TEMPLATE_SERVER$servercount/$server/g"
    let servercount=servercount+1
  done
done
    

