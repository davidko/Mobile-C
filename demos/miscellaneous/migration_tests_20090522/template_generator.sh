#!/bin/bash

#DEBUG=echo

if [ $# -lt 2 ] 
then
	echo "Usage: ./template_generator.sh <num_teams> <num_laps>"
	exit
fi

numhosts=`wc host_list.txt | awk ' { print $1 } '`

# Create the C-Style Hostlist
hostlist="{ "
for host in `cat host_list.txt`; do
	hostlist=$hostlist"\"$host\", "
done
hostlist=$hostlist"NULL }"

for team in `seq 1 $1`; do
  $DEBUG mkdir team$team
  let agent=1
  for server in `cat host_list.txt`; do
    agent_filename="agent"$agent"_team"$team".xml"
    $DEBUG cp agent_template.xml team$team/$agent_filename

    # Here, we use sed to replace some tags
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_AGENT_NAME/"agent"$agent"_team"$team/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_AGENT_NAME/"agent"$agent"_team"$team/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_AGENT_HOME/$server/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_INIT_HOST/$server/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_HOSTLIST/$hostlist/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_NUMHOSTS/$numhosts/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_INITPROGRESS/$(( $agent-1 ))/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_MAXLAPS/$2/g"
    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_AGENTNUMBER/$agent/g"

    let numagents=$numhosts+1
    let finallap_agentnumber=$numhosts*$2%$numagents
    let tourcount_stop=$numhosts*$2/$numagents 

    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_FINALLAP_AGENTNUMBER/$finallap_agentnumber/g"

    if [ "$agent" -eq 1 ]
    then
      $DEBUG cp team$team/$agent_filename team$team/agenttrigger_team$team.xml
      $DEBUG sed team$team/agenttrigger_team$team.xml -i -e "s/TEMPLATE_TRIGGER/1/g"
      $DEBUG sed team$team/agenttrigger_team$team.xml -i -e "s/agent1_team"$team"/agenttrigger_team"$team"/g"
      $DEBUG sed team$team/agenttrigger_team$team.xml -i -e "s/int agentnumber = 1;/int agentnumber = 0;/g"
      $DEBUG sed team$team/agenttrigger_team$team.xml -i -e "s/TEMPLATE_TOURCOUNT_STOP/$tourcount_stop/g"
			let cond1=100*$team+`perl -e "print $agent"`%$numagents
			let cond2=100*$team+`perl -e "print $agent - 1"`%$numagents
			#$DEBUG sed team$team/agenttrigger_team$team.xml -i -e "s/TEMPLATE_NEXTCOND/$cond1/g"
			#$DEBUG sed team$team/agenttrigger_team$team.xml -i -e "s/TEMPLATE_MYCOND/$cond2/g"
			$DEBUG sed team$team/agenttrigger_team$team.xml -i -e "s/TEMPLATE_NEXTAGENT_NAME/agent1_team$team/g"
    fi

		if [ "$agent" -ge $numhosts ] 
		then
			$DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_NEXTAGENT_NAME/agenttrigger_team$team/g"
		else
			let nextagent_number=$agent+1
			$DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_NEXTAGENT_NAME/agent"$nextagent_number"_team$team/g"
		fi


		let cond1=100*$team+`perl -e "print $agent + 1"`%$numagents
		let cond2=100*$team+$agent%$numagents
    #$DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_NEXTCOND/$cond1/g"
    #$DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_MYCOND/$cond2/g"

    if [ "$finallap_agentnumber" -ge "$agent" ]
    then
      $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_TOURCOUNT_STOP/$(($tourcount_stop+1))/g"
    else
      $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_TOURCOUNT_STOP/$tourcount_stop/g"
    fi

    $DEBUG sed team$team/$agent_filename -i -e "s/TEMPLATE_TRIGGER/0/g"

    let agent=$agent+1

  done
done
