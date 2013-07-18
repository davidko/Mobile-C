#!/usr/bin/env python

import mobilec
import mobilec.mc as mc
import os
import time
import threading

agencyoptions = mobilec.AgencyOptions()
ch_options = mobilec.ChOptions(chhome=os.environ['CHHOME'])
agencyoptions.ch_options = ch_options
agency = mobilec.Agency(port=5051, options=agencyoptions)

agent = mobilec.PyAgent("bob")
agency.registerPyAgent(agent)

def pingus():
  while True:
    print "Pingus!"
    time.sleep(2)

thread = threading.Thread(target=pingus)
thread.start()

while True:
  msg = agent.waitRetrieveMessage()
  if msg:
    print msg.getSenderName()

