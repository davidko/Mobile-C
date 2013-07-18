import mc

class Message():
  def __init__(self, message=None):
    if message is None:
      self._message = mc.fipa_acl_message_t()
    else:
      self._message = message

  def __getattr__(self, name):
    return getattr(mc._message, name)

  def getSenderName(self):
    rc = mc.MC_AclGetSenderName(self._message)
    return rc[1]

  def getSenderAddress(self):
    rc = mc.MC_AclGetSenderAddress(self._message)
    return rc[1]

  def getContent(self):
    rc = mc.MC_AclGetContent(self._message)
    return rc

  def getReply(self):
    reply = Message()
    rc = mc.MC_AclReply(self._message)
    reply._message = rc
    return reply

  def setProtocol(self, protocol):
    mc.MC_AclSetPrococol(self._message, protocol)

  def setConversationID(self, conv):
    mc.MC_AclSetConversationID(self._message, conv)

  def setPerformative(self, performative):
    mc.MC_AclSetPerformative(self._message, performative)

  def setSender(self, name, address):
    mc.MC_AclSetSender(self._message, name, address)

  def addReceiver(self, name, address):
    mc.MC_AclAddReceiver(self._message, name, address)

  def addReplyTo(self, name, address):
    mc.MC_AclAddReplyTo(self._message, name, address)

  def setContent(self, content):
    mc.MC_AclSetContent(self._message, content)

