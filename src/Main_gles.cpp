/*
*      Copyright (C) 2008-2016 Team Kodi
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
#include "Main_gles.h"
#endif


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

  // -- WavforHue function calls -------------------------------------
  // Initialize the lightIDs to something sane
  wavforhue.activeHueData.lightIDs.push_back("1");
  wavforhue.activeHueData.lightIDs.push_back("2");
  wavforhue.activeHueData.lightIDs.push_back("3");
  wavforhue.dimmedHueData.lightIDs.push_back("4");
  wavforhue.dimmedHueData.lightIDs.push_back("5");
  wavforhue.dimmedHueData.lightIDs.push_back("4");

  // Register this app with hue. It runs everytime you press play.
  // This allows something to press their bridge button within
  // 30 seconds to register this visualization with the Hue bridge.
  // Without registration, the Hue bridge won't accept the light
  // changing data from this visualization.
  wavforhue.RegisterHue();
  // Send the register command to the Hue bridge.
  transferQueue();

  //initialize the workaround for Cubox (imx6) HDMI
  if (wavforhue.cuboxHDMIFix)
  {
    wavforhue.iMaxAudioData_i = 180;
    wavforhue.fMaxAudioData = 179.0f;
  }
  // -- WavforHue function calls -------------------------------------


  return ADDON_STATUS_NEED_SAVEDSETTINGS;
}

//-- Start --------------------------------------------------------------------
// Called when a new soundtrack is played
//-----------------------------------------------------------------------------
extern "C" void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
{
  // -- WavforHue function calls -------------------------------------
  // Prepare lights - dimming, turning on, etc.
  wavforhue.Start();
  // -- WavforHue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  transferQueue();
  // -- Threading ---------------------------------------------------
}

//-- Stop ---------------------------------------------------------------------
// This dll must stop all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Stop()
{
  // -- WavforHue function calls -------------------------------------
  // Change the lights to something acceptable.
  wavforhue.Stop();
  // -- WavforHue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  transferQueue();
  // -- Threading ---------------------------------------------------

  //-- Threading -----------------------------------------------------
  gRunThread = false;
  while (gWorkerThread.joinable())  // Kill 'em all \m/
  {
    gWorkerThread.join();
  }
  //-- Threading -----------------------------------------------------  
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Destroy()
{

  //-- Threading -----------------------------------------------------
  gRunThread = false;
  while (gWorkerThread.joinable()) // Kill 'em all \m/
  {
    gWorkerThread.join();
  }
  //-- Threading -----------------------------------------------------
  
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
      wavforhue.sound.fWaveform[0][iPos] = float((pAudioData[i] / 32768.0f) * 255.0f);
      wavforhue.sound.fWaveform[1][iPos] = float((pAudioData[i + 1] / 32768.0f) * 255.0f);

      // damp the input into the FFT a bit, to reduce high-frequency noise:
      tempWave[0][iPos] = 0.5f * (wavforhue.sound.fWaveform[0][iPos] + wavforhue.sound.fWaveform[0][iOld]);
      tempWave[1][iPos] = 0.5f * (wavforhue.sound.fWaveform[1][iPos] + wavforhue.sound.fWaveform[1][iOld]);
      iOld = iPos;
      iPos++;
      if (iPos >= 576)
        break;
    }
  }

  // -- WavforHue function calls -------------------------------------
  wavforhue.fftobj.time_to_frequency_domain(tempWave[0], wavforhue.sound.fSpectrum[0]);
  wavforhue.fftobj.time_to_frequency_domain(tempWave[1], wavforhue.sound.fSpectrum[1]);
  wavforhue.AnalyzeSound();
  // -- WavforHue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  transferQueue();
  // -- Threading ---------------------------------------------------

}

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
extern "C" void Render()
{
  if (wavforhue.useWaveForm) {
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
    for (int i = 0; i < wavforhue.iMaxAudioData_i; i++)
    {
      col[i][0] = wavforhue.rgb[0];
      col[i][1] = wavforhue.rgb[1];
      col[i][2] = wavforhue.rgb[2];
      //ver[i][0] = g_viewport.X + ((i / 255.0f) * g_viewport.Width);
      //ver[i][1] = g_viewport.Y + g_viewport.Height * 0.33f + (g_fWaveform[0][i] * g_viewport.Height * 0.15f);
      ver[i][0] = -1.0f + ((i / wavforhue.fMaxAudioData) * 2.0f);
      ver[i][1] = 0.5f + wavforhue.fWaveform[0][i];
      ver[i][2] = 1.0f;
      idx[i] = i;
    }

    glDrawElements(GL_LINE_STRIP, wavforhue.iMaxAudioData_i, GL_UNSIGNED_BYTE, idx);

    // Right (lower) channel
    for (int i = 0; i < wavforhue.iMaxAudioData_i; i++)
    {
      col[i][0] = wavforhue.rgb[0];
      col[i][1] = wavforhue.rgb[1];
      col[i][2] = wavforhue.rgb[2];
      //ver[i][0] = g_viewport.X + ((i / 255.0f) * g_viewport.Width);
      //ver[i][1] = g_viewport.Y + g_viewport.Height * 0.66f + (g_fWaveform[1][i] * g_viewport.Height * 0.15f);
      ver[i][0] = -1.0f + ((i / wavforhue.fMaxAudioData) * 2.0f);
      ver[i][1] = -0.5f + wavforhue.fWaveform[1][i];
      ver[i][2] = 1.0f;
      idx[i] = i;

    }

    glDrawElements(GL_LINE_STRIP, wavforhue.iMaxAudioData_i, GL_UNSIGNED_BYTE, idx);

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
  
  // -- WavforHue function calls -------------------------------------
  //get some interesting numbers to play with
  wavforhue.UpdateTime();
  wavforhue.timePass = wavforhue.fElapsedAppTime;
  // -- WavforHue function calls -------------------------------------

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
    wavforhue.useWaveForm = *(bool*)value == 1;
  else if (strcmp(strSetting, "HueBridgeIP") == 0)
  {
    char* array;
    array = (char*)value;
    wavforhue.strHueBridgeIPAddress = std::string(array);
  }
  //----------------------------------------------------------  
  else if (strcmp(strSetting, "ActiveLights") == 0)
  {
    char* array;
    array = (char*)value;
    std::string activeLightIDsUnsplit = std::string(array);
    wavforhue.activeHueData.lightIDs.clear();
    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = activeLightIDsUnsplit.find(delimiter, last)) != std::string::npos)
    {
      wavforhue.activeHueData.lightIDs.push_back(activeLightIDsUnsplit.substr(last, next - last));
      last = next + 1;
    }
    //do the last light token
    wavforhue.activeHueData.lightIDs.push_back(activeLightIDsUnsplit.substr(last));
    wavforhue.activeHueData.numberOfLights = wavforhue.activeHueData.lightIDs.size();
  }
  else if (strcmp(strSetting, "BeatThreshold") == 0)
    wavforhue.beatThreshold = *(float*)value;
  else if (strcmp(strSetting, "MaxBri") == 0)
    wavforhue.maxBri = *(int*)value;
  else if (strcmp(strSetting, "HueRangeUpper") == 0)
  {
    wavforhue.lastHue = *(int*)value;
    wavforhue.initialHue = wavforhue.lastHue;
  }
  else if (strcmp(strSetting, "HueRangeLower") == 0)
    wavforhue.targetHue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "DimmedLights") == 0)
  {
    char* array;
    array = (char*)value;
    std::string dimmedLightIDsUnsplit = std::string(array);
    wavforhue.dimmedHueData.lightIDs.clear();
    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = dimmedLightIDsUnsplit.find(delimiter, last)) != std::string::npos)
    {
      wavforhue.dimmedHueData.lightIDs.push_back(dimmedLightIDsUnsplit.substr(last, next - last));
      last = next + 1;
    }
    //do the last light token
    wavforhue.dimmedHueData.lightIDs.push_back(dimmedLightIDsUnsplit.substr(last));
    if (wavforhue.dimmedHueData.lightIDs[0].size() == 0)
    {
      wavforhue.dimmedHueData.numberOfLights = 0;
    }
    else
    {
      wavforhue.dimmedHueData.numberOfLights = wavforhue.dimmedHueData.lightIDs.size();
    }
  }
  else if (strcmp(strSetting, "DimmedBri") == 0)
    wavforhue.dimmedHueData.bri = *(int*)value;
  else if (strcmp(strSetting, "DimmedSat") == 0)
    wavforhue.dimmedHueData.sat = *(int*)value;
  else if (strcmp(strSetting, "DimmedHue") == 0)
    wavforhue.dimmedHueData.hue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "LightsOnAfter") == 0)
    wavforhue.lightsOnAfter = *(bool*)value == 1;
  else if (strcmp(strSetting, "AfterLights") == 0)
  {
    char* array;
    array = (char*)value;
    std::string afterLightIDsUnsplit = std::string(array);
    wavforhue.afterHueData.lightIDs.clear();
    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = afterLightIDsUnsplit.find(delimiter, last)) != std::string::npos)
    {
      wavforhue.afterHueData.lightIDs.push_back(afterLightIDsUnsplit.substr(last, next - last));
      last = next + 1;
    }
    //do the last light token
    wavforhue.afterHueData.lightIDs.push_back(afterLightIDsUnsplit.substr(last));
    wavforhue.afterHueData.numberOfLights = wavforhue.afterHueData.lightIDs.size();
  }
  else if (strcmp(strSetting, "AfterBri") == 0)
    wavforhue.afterHueData.bri = *(int*)value;
  else if (strcmp(strSetting, "AfterSat") == 0)
    wavforhue.afterHueData.sat = *(int*)value;
  else if (strcmp(strSetting, "AfterHue") == 0)
    wavforhue.afterHueData.hue = *(int*)value;
  //----------------------------------------------------------
  else if (strcmp(strSetting, "CuboxHDMIFix") == 0)
    wavforhue.cuboxHDMIFix = *(bool*)value == 1;
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
