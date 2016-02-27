/*
*      Copyright (C) 2015-2016 Thomas M. Hardy
*      Copyright (C) 2003-2016 Team Kodi
*      Copyright (C) 1998-2000 Peter Alm, Mikael Alm, Olle Hallnas, 
*                              Thomas Nilsson and 4Front Technologies
*
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

/*
 *  Wed May 24 10:49:37 CDT 2000
 *  Fixes to threading/context creation for the nVidia X4 drivers by
 *  Christian Zander <phoenix@minion.de>
 */

/*
 *  Ported to GLES by gimli
 */

#ifndef WAVFORHUE_MAIN
#include "Main_gles.h"
#endif

WavforHue_Thread wt;

//-- Create -------------------------------------------------------------------
// Called on load. Addon should fully initalize or return error status
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!props)
    return ADDON_STATUS_UNKNOWN;

  // -- Waveform -----------------------------------------------------
  vis_shader = new CVisGUIShader(vert, frag);

  if (!vis_shader)
    return ADDON_STATUS_UNKNOWN;

  if (!vis_shader->CompileAndLink())
  {
    delete vis_shader;
    return ADDON_STATUS_UNKNOWN;
  }
  // -- Waveform -----------------------------------------------------

  // -- Wavforhue function calls -------------------------------------
  // Initialize the lightIDs to something sane
  wt.wavforhue.activeHueData.lightIDs.push_back("1");
  wt.wavforhue.activeHueData.lightIDs.push_back("2");
  wt.wavforhue.activeHueData.lightIDs.push_back("3");
  wt.wavforhue.dimmedHueData.lightIDs.push_back("4");
  wt.wavforhue.dimmedHueData.lightIDs.push_back("5");
  wt.wavforhue.dimmedHueData.lightIDs.push_back("4");

  // Register this app with hue. It runs everytime you press play.
  // This allows something to press their bridge button within
  // 30 seconds to register this visualization with the Hue bridge.
  // Without registration, the Hue bridge won't accept the light
  // changing data from this visualization.
  //wt.wavforhue.RegisterHue();
  // Send the register command to the Hue bridge.
  //wt.transferQueueToMain();
  // -- Wavforhue function calls -------------------------------------


  return ADDON_STATUS_NEED_SAVEDSETTINGS;
}

//-- Start --------------------------------------------------------------------
// Called when a new soundtrack is played
//-----------------------------------------------------------------------------
extern "C" void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
{
  // -- Wavforhue function calls -------------------------------------
  // Prepare lights - dimming, turning on, etc.
  wt.wavforhue.Start();
  if(wt.wavforhue.cuboxHDMIFix)
  {
    wt.wavforhue.iMaxAudioData_i = 180;
    wt.wavforhue.fMaxAudioData = 179.0f;
  }
  // -- Wavforhue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  wt.transferQueueToMain();
  // -- Threading ---------------------------------------------------
}

//-- Stop ---------------------------------------------------------------------
// This dll must stop all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Stop()
{
  // -- Wavforhue function calls -------------------------------------
  // Change the lights to something acceptable.
  wt.wavforhue.Stop();
  // -- Wavforhue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  wt.transferQueueToMain();
  // -- Threading ---------------------------------------------------

  //-- Threading -----------------------------------------------------
  wt.gRunThread = false;
  while (wt.gWorkerThread.joinable())  // Kill 'em all \m/
  {
    wt.gWorkerThread.join();
  }
  //-- Threading -----------------------------------------------------  
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Destroy()
{
  /*
  // -- Wavforhue function calls -------------------------------------
  // Change the lights to something acceptable.
  wt.wavforhue.Stop();
  // -- Wavforhue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  wt.transferQueue();
  // -- Threading ---------------------------------------------------

  //-- Threading -----------------------------------------------------
  wt.gRunThread = false;
  while (wt.gWorkerThread.joinable()) // Kill 'em all \m/
  {
    wt.gWorkerThread.join();
  }
  //-- Threading -----------------------------------------------------
  */  

  // -- Waveform -----------------------------------------------------
  if (vis_shader)
  {
    vis_shader->Free();
    delete vis_shader;
    vis_shader = NULL;
  }
  // -- Waveform -----------------------------------------------------

}

//-- Audiodata ----------------------------------------------------------------
// Called by XBMC to pass new audio data to the vis
//-----------------------------------------------------------------------------
extern "C" void AudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
{
  // Modified from Vortex
  float tempWave[2][576];
  int iPos = 0;
  int iOld = 0;

  while (iPos < 576)
  {
    for (int i = 0; i < iAudioDataLength; i += 2)
    {
      if(iPos < 512) // This is for Waveform.
      {
        g_fWaveform[0][iPos] = pAudioData[i  ]; // left channel
        g_fWaveform[1][iPos] = pAudioData[i+1]; // right channel
      }

      wt.wavforhue.sound.fWaveform[0][iPos] = float((pAudioData[i] / 32768.0f) * 255.0f);
      wt.wavforhue.sound.fWaveform[1][iPos] = float((pAudioData[i + 1] / 32768.0f) * 255.0f);

      // damp the input into the FFT a bit, to reduce high-frequency noise:
      tempWave[0][iPos] = 0.5f * (wt.wavforhue.sound.fWaveform[0][iPos] + wt.wavforhue.sound.fWaveform[0][iOld]);
      tempWave[1][iPos] = 0.5f * (wt.wavforhue.sound.fWaveform[1][iPos] + wt.wavforhue.sound.fWaveform[1][iOld]);
      iOld = iPos;
      iPos++;
      if (iPos >= 576)
        break;
    }
  }

  // -- Wavforhue function calls -------------------------------------
  wt.wavforhue.fftobj.time_to_frequency_domain(tempWave[0], wt.wavforhue.sound.fSpectrum[0]);
  wt.wavforhue.fftobj.time_to_frequency_domain(tempWave[1], wt.wavforhue.sound.fSpectrum[1]);
  wt.wavforhue.AnalyzeSound();
  // -- Wavforhue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  wt.transferQueueToThread();
  // -- Threading ---------------------------------------------------

}

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
extern "C" void Render()
{
  if (wt.wavforhue.useWaveForm) {
    // -- Waveform -----------------------------------------------------
    GLfloat col[256][3];
    GLfloat ver[256][3];
    GLubyte idx[256];

    glDisable(GL_BLEND);

    vis_shader->MatrixMode(MM_PROJECTION);
    vis_shader->PushMatrix();
    vis_shader->LoadIdentity();
    //vis_shader->Frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.5f, 10.0f);
    vis_shader->MatrixMode(MM_MODELVIEW);
    vis_shader->PushMatrix();
    vis_shader->LoadIdentity();

    vis_shader->PushMatrix();
    vis_shader->Translatef(0.0f, 0.0f, -1.0f);
    vis_shader->Rotatef(0.0f, 1.0f, 0.0f, 0.0f);
    vis_shader->Rotatef(0.0f, 0.0f, 1.0f, 0.0f);
    vis_shader->Rotatef(0.0f, 0.0f, 0.0f, 1.0f);

    vis_shader->Enable();

    GLint   posLoc = vis_shader->GetPosLoc();
    GLint   colLoc = vis_shader->GetColLoc();

    glVertexAttribPointer(colLoc, 3, GL_FLOAT, 0, 0, col);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, 0, 0, ver);

    glEnableVertexAttribArray(posLoc);
    glEnableVertexAttribArray(colLoc);

    // Left (upper) channel
    for (int i = 0; i < wt.wavforhue.iMaxAudioData_i; i++)
    {
      col[i][0] = wt.wavforhue.rgb[0];
      col[i][1] = wt.wavforhue.rgb[1];
      col[i][2] = wt.wavforhue.rgb[2];
      //ver[i][0] = g_viewport.X + ((i / 255.0f) * g_viewport.Width);
      //ver[i][1] = g_viewport.Y + g_viewport.Height * 0.33f + (g_fWaveform[0][i] * g_viewport.Height * 0.15f);
      ver[i][0] = -1.0f + ((i / wt.wavforhue.fMaxAudioData) * 2.0f);
      ver[i][1] = 0.5f + g_fWaveform[0][i];
      ver[i][2] = 1.0f;
      idx[i] = i;
    }

    glDrawElements(GL_LINE_STRIP, wt.wavforhue.iMaxAudioData_i, GL_UNSIGNED_BYTE, idx);

    // Right (lower) channel
    for (int i = 0; i < wt.wavforhue.iMaxAudioData_i; i++)
    {
      col[i][0] = wt.wavforhue.rgb[0];
      col[i][1] = wt.wavforhue.rgb[1];
      col[i][2] = wt.wavforhue.rgb[2];
      //ver[i][0] = g_viewport.X + ((i / 255.0f) * g_viewport.Width);
      //ver[i][1] = g_viewport.Y + g_viewport.Height * 0.66f + (g_fWaveform[1][i] * g_viewport.Height * 0.15f);
      ver[i][0] = -1.0f + ((i / wt.wavforhue.fMaxAudioData) * 2.0f);
      ver[i][1] = -0.5f + g_fWaveform[1][i];
      ver[i][2] = 1.0f;
      idx[i] = i;

    }

    glDrawElements(GL_LINE_STRIP, wt.wavforhue.iMaxAudioData_i, GL_UNSIGNED_BYTE, idx);

    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(colLoc);

    vis_shader->Disable();

    vis_shader->PopMatrix();

    vis_shader->PopMatrix();
    vis_shader->MatrixMode(MM_PROJECTION);
    vis_shader->PopMatrix();

    glEnable(GL_BLEND);
    // -- Waveform -----------------------------------------------------
  }
  
  // -- Wavforhue function calls -------------------------------------
  //get some interesting numbers to play with
  wt.wavforhue.UpdateTime();
  wt.wavforhue.timePass = wt.wavforhue.fElapsedAppTime;
  // -- Wavforhue function calls -------------------------------------

}

//-- GetInfo ------------------------------------------------------------------
// Tell XBMC our requirements
//-----------------------------------------------------------------------------
extern "C" void GetInfo(VIS_INFO* pInfo)
{
  pInfo->bWantsFreq = false;
  pInfo->iSyncDelay = 0;
}

//-- GetSubModules ------------------------------------------------------------
// Return any sub modules supported by this vis
//-----------------------------------------------------------------------------
extern "C" unsigned int GetSubModules(char ***names)
{
  return 0; // this vis supports 0 sub modules
}

//-- OnAction -----------------------------------------------------------------
// Handle XBMC actions such as next preset, lock preset, album art changed etc
//-----------------------------------------------------------------------------
extern "C" bool OnAction(long flags, const void *param)
{
  bool ret = false;
  return ret;
}

//-- GetPresets ---------------------------------------------------------------
// Return a list of presets to XBMC for display
//-----------------------------------------------------------------------------
extern "C" unsigned int GetPresets(char ***presets)
{
  return 0;
}

//-- GetPreset ----------------------------------------------------------------
// Return the index of the current playing preset
//-----------------------------------------------------------------------------
extern "C" unsigned GetPreset()
{
  return 0;
}

//-- IsLocked -----------------------------------------------------------------
// Returns true if this add-on use settings
//-----------------------------------------------------------------------------
extern "C" bool IsLocked()
{
  return true;
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" bool ADDON_HasSettings()
{
  return true;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------

extern "C" void ADDON_FreeSettings()
{
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  if (!strSetting || !value)
    return ADDON_STATUS_UNKNOWN;

  if (strcmp(strSetting, "UseWaveForm") == 0)
    wt.wavforhue.useWaveForm = *(bool*)value == 1;
  else if (strcmp(strSetting, "HueBridgeIP") == 0)
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
  else if (strcmp(strSetting, "ActiveLights") == 0)
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
  else if (strcmp(strSetting, "BeatThreshold") == 0)
    wt.wavforhue.beatThreshold = *(float*)value;
  else if (strcmp(strSetting, "MaxBri") == 0)
    wt.wavforhue.maxBri = *(int*)value;
  else if (strcmp(strSetting, "HueRangeUpper") == 0)
  {
    wt.wavforhue.lastHue = *(int*)value;
    wt.wavforhue.initialHue = wt.wavforhue.lastHue;
  }
  else if (strcmp(strSetting, "HueRangeLower") == 0)
    wt.wavforhue.targetHue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "DimmedLights") == 0)
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
  else if (strcmp(strSetting, "DimmedBri") == 0)
    wt.wavforhue.dimmedHueData.bri = *(int*)value;
  else if (strcmp(strSetting, "DimmedSat") == 0)
    wt.wavforhue.dimmedHueData.sat = *(int*)value;
  else if (strcmp(strSetting, "DimmedHue") == 0)
    wt.wavforhue.dimmedHueData.hue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "AfterLights") == 0)
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
  else if (strcmp(strSetting, "AfterBri") == 0)
    wt.wavforhue.afterHueData.bri = *(int*)value;
  else if (strcmp(strSetting, "AfterSat") == 0)
    wt.wavforhue.afterHueData.sat = *(int*)value;
  else if (strcmp(strSetting, "AfterHue") == 0)
    wt.wavforhue.afterHueData.hue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "CuboxHDMIFix") == 0)
    wt.wavforhue.cuboxHDMIFix = *(bool*)value == 1;
  else
    return ADDON_STATUS_UNKNOWN;

  return ADDON_STATUS_OK;
}

//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}
