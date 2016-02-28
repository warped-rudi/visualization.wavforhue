import xbmc
import xbmcgui
import xbmcaddon
import json
import time
import sys
import colorsys
import os
import datetime
import math
from threading import Timer

__addon__      = xbmcaddon.Addon('visualization.wavforhue')
__addondir__   = xbmc.translatePath( __addon__.getAddonInfo('profile') ) 
__cwd__        = __addon__.getAddonInfo('path')
__resource__   = xbmc.translatePath( os.path.join( __cwd__, 'resources', 'lib' ) )
__icon__       = os.path.join(__cwd__,"icon.png")

sys.path.append (__resource__)

from settings import *
from tools import *

try:
  import requests
except ImportError:
  xbmc.log("ERROR: Could not locate required library requests")
  notify("WavforHue", "ERROR: Could not import Python requests")

  


# class settings():
  # def __init__( self, *args, **kwargs ):
    # self.readxml()
    # self.addon = xbmcaddon.Addon('visualization.wavforhue')

  # def readxml(self):
    # self.bridge_ip             = __addon__.getSetting("HueBridgeIP")
 
  # def update(self, **kwargs):
    # self.__dict__.update(**kwargs)
    # for k, v in kwargs.iteritems():
      # self.addon.setSetting(k, v)

  # def __repr__(self):
    # return 'bridge_ip: %s\n' % self.bridge_ip
 
# def notify(title, msg=""):
  # if not NOSE:
    # global __icon__
    # xbmc.executebuiltin("XBMC.Notification(%s, %s, 3, %s)" % (title, msg, __icon__))
 
# class MyClass(xbmcgui.Window):
  # def __init__(self):
    # self.settings = settings()
    # notify("Bridge discovery", "starting")
    # self.settings.update(HueBridgeIP = "192.168.10.7")
 

if ( __name__ == "__main__" ):
  settings = settings()
  logger = Logger()
  #monitor = MyMonitor()
  if settings.debug == True:
    logger.debug()
  
  args = None
  if len(sys.argv) == 2:
    args = sys.argv[1]
  hue = Hue(settings, args)
  # Find all the light IDs.
  # Loop through all the lights.
    # Flash current light.
    # Ask user if they want the light to be active, dimmed, or off