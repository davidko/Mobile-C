#!/usr/bin/env python

import mobilec
import os
import sys

sys.setdlopenflags( 0x100 | 0x2 )

from ctypes import *
libmc = CDLL("libmc.so")

agencyoptions = mobilec.AgencyOptions()
ch_options = mobilec.ChOptions(chhome=os.environ['CHHOME'])
agencyoptions.ch_options = ch_options
agency = mobilec.Agency(port=5051, options=agencyoptions)
raw_input('Press any key to quit the server...')

