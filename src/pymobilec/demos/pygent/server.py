#!/usr/bin/env python

import mobilec
import mobilec.mc as mc
import os

agencyoptions = mobilec.AgencyOptions()
ch_options = mobilec.ChOptions(chhome=os.environ['CHHOME'])
agencyoptions.ch_options = ch_options
agency = mobilec.Agency(port=5051, options=agencyoptions)

agent = mobilec.PyAgent("bob")
agency.registerPyAgent(agent)
while True:
  key = raw_input('Type "q" to quit, or enter to check for messages')
  if key == 'q':
    break
  msg = agent.retrieveMessage()
  if msg:
    print msg.getSenderName()

