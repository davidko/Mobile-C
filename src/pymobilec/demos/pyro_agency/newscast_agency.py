#!/usr/bin/env python

import mobilec
import os
import threading
import time

from newscast import NewsCastPeer

class NewscastAgency(mobilec.Agency, NewsCastPeer):
  def __init__(self, hostname="localhost", port=5050, options=None):
    mobilec.Agency.__init__(self, port, options)
    NewsCastPeer.__init__(self, hostname+":{0}".format(port))
    self.hostname = hostname
    self.port = port

  def getAddress(self):
    return "{0}:{1}".format(self.hostname, self.port)

class NewscastManager(mobilec.PyAgent):
  def __init__(self, agency, name="Manager"):
    mobilec.PyAgent.__init__(self, name)
    self.myname = name
    self.newscastagency = agency
    self.newscastagency.registerPyAgent(self)

  def start_thread(self):
    def threadfunc():
      while True:
        self.handle_message()
    thread = threading.Thread(target=threadfunc)
    thread.daemon = True
    thread.start()
  
  def handle_message(self):
    msg = self.waitRetrieveMessage()
    
    content = msg.getContent()
    if content == 'get computer load':
      reply_content = repr(os.getloadavg())
    elif content == 'get agency address':
      reply_content = self.newscastagency.getAddress()
    elif content == 'get peers':
      print self.newscastagency.peers
      addresses = [n for _,_,n in self.newscastagency.peers]
      reply_content = "%".join(addresses)

    reply = msg.getReply()
    reply.setContent(reply_content)
    reply.setSender(self.myname, "http://"+self.newscastagency.getAddress()+"/acc")
    reply.setPerformative(mobilec.FIPA_INFORM)
    self.newscastagency.sendMessage(reply)


def main():
  import Pyro4
  daemon = Pyro4.Daemon()
  agencyoptions = mobilec.AgencyOptions()
  ch_options = mobilec.ChOptions(chhome=os.environ['CHHOME'])
  agencyoptions.ch_options = ch_options
  agency = NewscastAgency(options=agencyoptions)
  master_uri = daemon.register(agency, objectId="mobilec5050").asString()
  print "Master agency at {0}".format(master_uri)
  agency.set_uri(master_uri)

  manager = NewscastManager(agency)
  manager.start_thread()
  agency.start_newscast()

  agencies = []
  for i in range(10):
    a = NewscastAgency(port=5051+i, options=agencyoptions)
    agencies.append(a)
    m = NewscastManager(a)
    m.start_thread()
    uri = daemon.register(a, objectId="mobilec"+str(5051+i)).asString()
    a.set_uri(uri)
    a.set_peers([(time.time(), master_uri, agency.getAddress())])
    a.start_newscast()

  daemonthread = threading.Thread(target=daemon.requestLoop)
  daemonthread.daemon = True
  daemonthread.start()

  # Start an agent
  agent = mobilec.Agent()
  agent.initFromFile(
      "MySimpleAgent",
      "localhost:5050",
      "David",
      "traveller.c",
      None,
      "localhost:5051")
  agency.addAgent(agent)

  raw_input('Press enter to quit')

if __name__=='__main__':
  main()
