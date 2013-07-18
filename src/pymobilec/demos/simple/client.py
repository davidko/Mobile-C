#!/usr/bin/env python

import mobilec
import os

agencyoptions = mobilec.AgencyOptions()
ch_options = mobilec.ChOptions(chhome=os.environ['CHHOME'])
agencyoptions.ch_options = ch_options

agency = mobilec.Agency(port=5050, options=agencyoptions)
agent = mobilec.Agent()
agent.initFromFile(
    "MySimpleAgent",
    "localhost:5050",
    "David",
    "hello_world.c",
    None,
    "localhost:5051")
agency.addAgent(agent)
raw_input('Press any key to quit the server...')
    
