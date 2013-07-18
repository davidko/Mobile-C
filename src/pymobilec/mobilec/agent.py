import mc

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
    return mc.MC_AclRetrieve(self)

class PyAgent(mc.stationary_agent_info_t):
  def __init__(self):
    mc.stationary_agent_info_t.__init__(self)

  def retrieveMessage(self):
    return mc.MC_AclRetrieve(self.agent)

