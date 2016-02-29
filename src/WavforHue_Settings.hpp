/*
*      Copyright (C) 2015-2016 Thomas M. Hardy
*      Copyright (C) 2003-2016 Team Kodi
*      http://kodi.tv
*
*  This Program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*
*  This Program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with XBMC; see the file COPYING.  If not, see
*  <http://www.gnu.org/licenses/>.
*
*/

#ifndef WAVFORHUE_MAIN
#include "Main.h"
#endif

using namespace ADDON;

extern "C" ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  if (!strSetting || !value)
    return ADDON_STATUS_UNKNOWN;

  if (strcmp(strSetting, "useWaveForm") == 0)
    wt.wavforhue.useWaveForm = *(bool*)value == 1;
  else if (strcmp(strSetting, "hueBridgeIP") == 0)
  {
    char* array;
    array = (char*)value;
    wt.wavforhue.strHueBridgeIPAddress = std::string(array);
  }
  else if (strcmp(strSetting, "hueBridgeUser") == 0)
  {
    char* array;
    array = (char*)value;
    wt.wavforhue.strHueBridgeUser = std::string(array);
  }
  //---------------------------------------------------------- 
  else if (strcmp(strSetting, "priorState") == 0)
    wt.wavforhue.priorState = *(bool*)value == 1;
  else if (strcmp(strSetting, "activeLights") == 0)
  {
    char* array;
    array = (char*)value;
    std::string activeLightIDsUnsplit = std::string(array);
    wt.wavforhue.activeHueData.lightIDs.clear();
    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = activeLightIDsUnsplit.find(delimiter, last)) != std::string::npos)
    {
      wt.wavforhue.activeHueData.lightIDs.push_back(activeLightIDsUnsplit.substr(last, next - last));
      last = next + 1;
    }
    //do the last light token
    wt.wavforhue.activeHueData.lightIDs.push_back(activeLightIDsUnsplit.substr(last));
    wt.wavforhue.activeHueData.numberOfLights = wt.wavforhue.activeHueData.lightIDs.size();
  }
  else if (strcmp(strSetting, "beatThreshold") == 0)
    wt.wavforhue.beatThreshold = *(float*)value;
  else if (strcmp(strSetting, "maxBri") == 0)
    wt.wavforhue.maxBri = *(int*)value;
  else if (strcmp(strSetting, "hueRangeUpper") == 0)
  {
    wt.wavforhue.lastHue = *(int*)value;
    wt.wavforhue.initialHue = wt.wavforhue.lastHue;
  }
  else if (strcmp(strSetting, "hueRangeLower") == 0)
    wt.wavforhue.targetHue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "dimmedLights") == 0)
  {
    char* array;
    array = (char*)value;
    std::string dimmedLightIDsUnsplit = std::string(array);
    wt.wavforhue.dimmedHueData.lightIDs.clear();
    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = dimmedLightIDsUnsplit.find(delimiter, last)) != std::string::npos)
    {
      wt.wavforhue.dimmedHueData.lightIDs.push_back(dimmedLightIDsUnsplit.substr(last, next - last));
      last = next + 1;
    }
    //do the last light token
    wt.wavforhue.dimmedHueData.lightIDs.push_back(dimmedLightIDsUnsplit.substr(last));
    if (wt.wavforhue.dimmedHueData.lightIDs[0].size() == 0)
    {
      wt.wavforhue.dimmedHueData.numberOfLights = 0;
    }
    else
    {
      wt.wavforhue.dimmedHueData.numberOfLights = wt.wavforhue.dimmedHueData.lightIDs.size();
    }
  }
  else if (strcmp(strSetting, "dimmedBri") == 0)
    wt.wavforhue.dimmedHueData.bri = *(int*)value;
  else if (strcmp(strSetting, "dimmedSat") == 0)
    wt.wavforhue.dimmedHueData.sat = *(int*)value;
  else if (strcmp(strSetting, "dimmedHue") == 0)
    wt.wavforhue.dimmedHueData.hue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "afterLights") == 0)
  {
    char* array;
    array = (char*)value;
    std::string afterLightIDsUnsplit = std::string(array);
    wt.wavforhue.afterHueData.lightIDs.clear();
    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = afterLightIDsUnsplit.find(delimiter, last)) != std::string::npos)
    {
      wt.wavforhue.afterHueData.lightIDs.push_back(afterLightIDsUnsplit.substr(last, next - last));
      last = next + 1;
    }
    //do the last light token
    wt.wavforhue.afterHueData.lightIDs.push_back(afterLightIDsUnsplit.substr(last));
    if (wt.wavforhue.afterHueData.lightIDs[0].size() == 0)
    {
      wt.wavforhue.afterHueData.numberOfLights = 0;
    }
    else
    {
      wt.wavforhue.afterHueData.numberOfLights = wt.wavforhue.afterHueData.lightIDs.size();
    }
  }
  else if (strcmp(strSetting, "afterBri") == 0)
    wt.wavforhue.afterHueData.bri = *(int*)value;
  else if (strcmp(strSetting, "afterSat") == 0)
    wt.wavforhue.afterHueData.sat = *(int*)value;
  else if (strcmp(strSetting, "afterHue") == 0)
    wt.wavforhue.afterHueData.hue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "cuboxHDMIFix") == 0)
    wt.wavforhue.cuboxHDMIFix = *(bool*)value == 1;
  else if (strcmp(strSetting, "config") == 0)
    // do nothing
    ((void)0);
  else if (strcmp(strSetting, "reset_settings") == 0)
    // do nothing
    ((void)0);
  else if (strcmp(strSetting, "config_lights") == 0)
    // do nothing
    ((void)0); 
  else if (strcmp(strSetting, "debug") == 0)
    wt.wavforhue.debug = *(bool*)value == 1;
  else if (strcmp(strSetting, "###GetSavedSettings") == 0)
    // wtf
    return ADDON_STATUS_UNKNOWN;
  else
  {
    char* array;
    array = (char*)strSetting;
    std::string badSetting = "Got unknown setting " + std::string(array);
    XBMC->Log(LOG_DEBUG, badSetting.c_str());
    return ADDON_STATUS_UNKNOWN;
  }

  return ADDON_STATUS_OK;
}