#!/usr/bin/env python

import Pyro4
import time
import random
import threading

class NewsCastPeer():
  MAX_PEERS = 10
  BROADCAST_INTERVAL = 5
  def __init__(self, name):
    self.peers = [] # List of tuples (time, uri, name)
    self.myname = name
    self.uri = None

  def set_uri(self, uri):
    self.uri = uri

  def get_uri(self):
    return self.uri

  def get_peers(self):
    return self.peers + [(time.time(), self.uri, self.myname)]

  def set_peers(self, peers):
    self.peers += peers
    self.peers.sort(key=lambda peer: peer[0], reverse=True)
    self._filter_duplicates()
    self.peers = self.peers[0:self.MAX_PEERS]

  def transact_with_other(self, uri):
    try:
      other = Pyro4.Proxy(uri[1])
    except:
      self.peers = filter(lambda peer: peer[1] != uri, self.peers)
      raise
    # Get the other guy's list of peers
    otherpeers = other.get_peers()
    self.peers += otherpeers
    self._filter_duplicates()
    self.peers = self.peers[0:self.MAX_PEERS]
    # Push our peers onto the other guy
    other.set_peers(self.peers + [(time.time(), self.uri, self.myname)])
    
  def _filter_duplicates(self):
    newtimes = []
    newuris = []
    newnames = []
    self.peers.sort(key=lambda peer: peer[0], reverse=True)
    for time,uri,name in self.peers:
      if uri in newuris:
        continue
      newtimes.append(time)
      newuris.append(uri)
      newnames.append(name)
    self.peers = zip(newtimes, newuris, newnames)

  def start_newscast(self):
    def thread_func():
      while True:
        time.sleep(self.BROADCAST_INTERVAL + random.random()*self.BROADCAST_INTERVAL)
        if len(self.peers) > 0:
          peer = random.choice(self.peers)
          self.transact_with_other(peer)

    thread = threading.Thread(target=thread_func)
    thread.setDaemon(True)
    thread.start()


def main():
  daemon = Pyro4.Daemon()
  peers = []
  uris = []
  for i in range(10):
    p = NewsCastPeer()
    uri = daemon.register(p).asString() 
    p.set_uri(uri)
    peers.append(p)
    uris.append(uri)
    if i > 0:
      p.set_peers([(time.time(), peers[0].get_uri())])

  for peer in peers:
    peer.start_newscast()

  print uris

  daemonthread = threading.Thread(target=daemon.requestLoop)
  daemonthread.daemon = True
  daemonthread.start()

  while True:
    print peers[0].get_peers()
    time.sleep(3)

if __name__ == '__main__':
  main()
