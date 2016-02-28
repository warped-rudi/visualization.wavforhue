import sys
import xbmcaddon

__addon__      = sys.modules[ "__main__" ].__addon__

class settings():
  def __init__( self, *args, **kwargs ):
    self.readxml()
    self.addon = xbmcaddon.Addon()

  def readxml(self):
    self.hueBridgeIP           = __addon__.getSetting("hueBridgeIP")
    self.hueBridgeUser         = __addon__.getSetting("hueBridgeUser")

    self.activeLights          = __addon__.getSetting("activeLights")
    self.dimmedLights          = __addon__.getSetting("dimmedLights")
    self.afterLights           = __addon__.getSetting("afterLights")
    self.priorState            = __addon__.getSetting("priorState") == "false"

    
    if self.ambilight_min > self.ambilight_max:
        self.ambilight_min = self.ambilight_max
        __addon__.setSetting("ambilight_min", __addon__.getSetting("ambilight_max"))

    self.debug                 = __addon__.getSetting("debug") == "true"

  def update(self, **kwargs):
    self.__dict__.update(**kwargs)
    for k, v in kwargs.iteritems():
      self.addon.setSetting(k, v)

  def __repr__(self):
    return 'hueBridgeIP: %s\n' % self.hueBridgeIP + \
    'hueBridgeUser: %s\n' % self.hueBridgeUser + \
    'activeLights: %s\n' % str(self.activeLights) + \
    'dimmedLights: %s\n' % str(self.dimmedLights) + \
    'afterLights: %s\n' % str(self.afterLights) + \
    'priorState: %s\n' % str
    'debug: %s\n' % self.debug