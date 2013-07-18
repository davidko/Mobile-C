#!/usr/bin/env python

import mobilec
import os

agencyoptions = mobilec.AgencyOptions()
ch_options = mobilec.ChOptions(chhome=os.environ['CHHOME'])
agencyoptions.ch_options = ch_options

agency = mobilec.Agency(port=5050, options=agencyoptions)

while True:
  key = raw_input('Press "q" to quit, or any other key to send a message')
  if key == 'q':
    break
  message = mobilec.Message()
  message.setPerformative(mobilec.FIPA_INFORM)
  message.setSender("joe", "http://localhost:5050/acc")
  message.addReceiver("bob", "http://localhost:5051/acc")
  message.setContent("Hello bob!")
  agency.sendMessage(message)
    
