#!/usr/bin/env python

HOSTLIST=["bird2.engr.ucdavis.edu:5051", "rabbit.engr.ucdavis.edu:5051"]

""" 
We need to start with one agent on each agency, and one extra agent to start the thing off.
The agents will all need to keep running for the required number of laps. 

Each time an agent moves, it will increment a counter. If the counter equals the number
of required laps _and_ the agent is on the home agency, then it was the anchor and 
it is done racing. 
"""

