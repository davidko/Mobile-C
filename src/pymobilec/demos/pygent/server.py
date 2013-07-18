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

def worker():
  while True:
    msg = agent.waitRetrieveMessage()
    if msg:
      print msg.getSenderName()
      print msg.getContent()
    reply = msg.getReply()
    reply.setContent("Hello back to you!")
    reply.setSender("bob", "http://localhost:5051/acc")
    reply.setPerformative(mobilec.FIPA_INFORM)
    agency.sendMessage(reply)

thread = threading.Thread(target=worker)
thread.daemon = True
thread.start()

raw_input("Press enter to quit.")
