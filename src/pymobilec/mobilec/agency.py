import mc
import time

class AgencyOptions(mc.MCAgencyOptions_t):
  def __init__(self,
        threads=0xFFFF & ~(1<<mc.MC_THREAD_CP),
        default_agent_status=mc.MC_WAIT_CH,
        enable_security=0,
        stack_size=-1,
        known_host_filename=None,
        priv_key_filename=None,
        initInterps=4,
        bluetooth=0,
        ch_options=None):
    mc.MCAgencyOptions_t.__init__(self)
    mc.MC_InitializeAgencyOptions(self)
#self = mc.MC_AgencyOptions_New()

    self.threads = threads
    self.default_agent_status = default_agent_status
    self.modified = 0
    self.enable_security = enable_security
    self.known_host_filename = known_host_filename
    self.priv_key_filename = priv_key_filename
    self.initInterps = initInterps
    self.bluetooth = bluetooth
    self.ch_options = ch_options
    self.initialized = mc.MC_INITIALIZED_CODE

class ChOptions(mc.ChOptions_t):
  def __init__(self, 
      shelltype=0,
      chhome=None,
      licensestr=None,
      chmtdir=None):
    mc.ChOptions_t.__init__(self)
    self.shelltype = shelltype
    self.chhome = chhome
    self.licensestr=licensestr
    self.chmtdir=chmtdir

class Agency():
  def __init__(self, port=5050, options=None):
    """
    Create a Mobile-C agency. Optional argument "options" can be an instance of
    MCAgencyOptions_t or None.
    """
    self._agency = mc.MC_Initialize(port, options)

  def addAgent(self, agent):
    mc.MC_AddAgent(self._agency, agent)

  def getNumAgents(self):
    return mc.MC_GetAllAgents(self._agency)

  def mainloop(self):
    while mc.MC_QuitFlag(self._agency) == 0:
      time.sleep(0.5)

  def end(self):
    mc.MC_End(self._agency)

  def registerPyAgent(self, pyagent):
    mc.MC_StationaryAgent_Register(self._agency, pyagent, pyagent.name)

  def sendMessage(self, message):
    mc.MC_AclSend(self._agency, message._message)
