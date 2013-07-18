import mc
import message

class Agent(mc.agent_s):
  def __init__(self):
    mc.agent_s.__init__(self)
    mc.agent_Init(self)

  def compose(self, name, home, owner, code, return_var_name, server, persistent=False):
    self.copy(mc.MC_ComposeAgent(name, home, owner, code, return_var_name, server, persistent))

  def initFromFile(self, name, home, owner, filename, return_var_name, server, persistent=False, workgroup=None):
    mc.MC_InitializeAgentFromFileWithWorkgroup(
        self, name, home, owner, filename, return_var_name, server, persistent, workgroup)

  def retrieveMessage(self):
    msg = mc.MC_AclRetrieve(self)
    if msg is not None:
      return message.Message(msg)
    else:
      return None

class PyAgent(mc.stationary_agent_info_t):
  def __init__(self, name):
    mc.stationary_agent_info_t.__init__(self)
    self.name = name

  def retrieveMessage(self):
    msg = mc.MC_AclRetrieve(self.agent)
    if msg is not None:
      return message.Message(msg)
    else:
      return None

  def waitRetrieveMessage(self):
    msg = mc.MC_AclWaitRetrieve(self.agent)
    if msg is not None:
      return message.Message(msg)
    else:
      return None


