#!/usr/bin/env python

import mobilec
import os
import Pyro4
import time

agencyoptions = mobilec.AgencyOptions()
ch_options = mobilec.ChOptions(chhome=os.environ['CHHOME'])
agencyoptions.ch_options = ch_options
agency = mobilec.Agency(port=5051, options=agencyoptions)
raw_input('Press any key to quit the server...')

daemon = Pyro4.Daemon()
agency_uri = daemon.register(agency)
ns = Pyro4.locateNS()
ns.register("example.mobilec.agency", agency_uri)
daemon.requestLoop()
