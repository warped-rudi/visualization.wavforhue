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

#ifndef WAVFORHUE_MAIN
#include "Main.h"
#endif

#ifndef WAVFORHUE_THREAD
#include "WavforHue_Thread.h"
#endif

#ifndef WAVFORHUE
#include "WavforHue.h"
#endif

using namespace ADDON;

WavforHue_Thread wt;

//-- Create -------------------------------------------------------------------
// Called on load. Addon should fully initalize or return error status
//-----------------------------------------------------------------------------
extern "C" ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!props)
    return ADDON_STATUS_UNKNOWN;

  XBMC = new CHelper_libXBMC_addon;
  if (!XBMC->RegisterMe(hdl)) {
    SAFE_DELETE(XBMC);
    return ADDON_STATUS_PERMANENT_FAILURE;
  }

  // -- Waveform -----------------------------------------------------
  VIS_PROPS* visProps = (VIS_PROPS*)props;

  
  /*
#ifdef HAS_OPENGL
  g_device = visProps->device;
#endif
  g_viewport.TopLeftX = (float)visProps->x;
  g_viewport.TopLeftY = (float)visProps->y;
  g_viewport.Width = (float)visProps->width;
  g_viewport.Height = (float)visProps->height;
  g_viewport.MinDepth = 0;
  g_viewport.MaxDepth = 1;
#ifndef HAS_OPENGL  
  g_context = (ID3D11DeviceContext*)visProps->device;
  g_context->GetDevice(&g_device);
  if (!init_renderer_objs())
    return ADDON_STATUS_PERMANENT_FAILURE;
#endif
  */
  
#ifndef HAS_OPENGL  
  g_device = (LPDIRECT3DDEVICE9)visProps->device;
#else
  g_device = visProps->device;
#endif
  g_viewport.X = visProps->x;
  g_viewport.Y = visProps->y;
  g_viewport.Width = visProps->width;
  g_viewport.Height = visProps->height;
  g_viewport.MinZ = 0;
  g_viewport.MaxZ = 1;
  // -- Waveform -----------------------------------------------------

  // -- WavforHue function calls -------------------------------------
  // Initialize the lightIDs to something sane
  wt.wavforhue.activeHueData.lightIDs.push_back("1");
  wt.wavforhue.activeHueData.lightIDs.push_back("2");
  wt.wavforhue.activeHueData.lightIDs.push_back("3");
  wt.wavforhue.dimmedHueData.lightIDs.push_back("4");
  wt.wavforhue.dimmedHueData.lightIDs.push_back("5");
  wt.wavforhue.afterHueData.lightIDs.push_back("4");
  // -- WavforHue function calls -------------------------------------

  return ADDON_STATUS_NEED_SAVEDSETTINGS;
}

//-- Start --------------------------------------------------------------------
// Called when a new soundtrack is played
//-----------------------------------------------------------------------------
extern "C" void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
{
  // Remove pending jobs
  wt.DiscardWorkerJobs();

  // -- WavforHue function calls -------------------------------------
  // Prepare lights - dimming, turning on, etc.
  XBMC->Log(LOG_DEBUG, "Preparing lights");
  if (!wt.wavforhue.savedTheStates)
  {
    XBMC->Log(LOG_DEBUG, "No previous states saved.");

    wt.StopWorker();
    wt.GetPriorState();
  }
  XBMC->Log(LOG_DEBUG, "Applying starting light settings.");
  wt.wavforhue.Start();
  // -- WavforHue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  XBMC->Log(LOG_DEBUG, "Transfering queue to thread.");
  wt.TransferQueueToThread();
  wt.StartWorker();
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
  if (wt.wavforhue.priorState)
    wt.wavforhue.RestoreState();
  else
    wt.wavforhue.Stop();
  // -- WavforHue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Remove pending jobs
  wt.DiscardWorkerJobs();
  // Put this/these light request on the thread's queue.
  wt.TransferQueueToThread();
  // -- Threading ---------------------------------------------------
}

//-- Detroy -------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Destroy()
{
  // -- Threading ---------------------------------------------------
  wt.StopWorker();
  // -- Threading ---------------------------------------------------

  if (XBMC)
    SAFE_DELETE(XBMC);

  // -- Waveform -----------------------------------------------------
  /*
#ifndef HAS_OPENGL
  if (g_cViewPort)
    g_cViewPort->Release();
  if (g_vBuffer)
    g_vBuffer->Release();
  if (g_inputLayout)
    g_inputLayout->Release();
  if (g_vShader)
    g_vShader->Release();
  if (g_pShader)
    g_pShader->Release();
  if (g_device)
    g_device->Release();
#endif
  */
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

  // -- WavforHue function calls -------------------------------------
  wt.wavforhue.fftobj.time_to_frequency_domain(tempWave[0], wt.wavforhue.sound.fSpectrum[0]);
  wt.wavforhue.fftobj.time_to_frequency_domain(tempWave[1], wt.wavforhue.sound.fSpectrum[1]);
  wt.wavforhue.AnalyzeSound();
  // -- WavforHue function calls -------------------------------------

  // -- Threading ---------------------------------------------------
  // Put this/these light request on the thread's queue.
  wt.TransferQueueToThread();
  // -- Threading ---------------------------------------------------
}

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
extern "C" void Render()
{
  if (wt.wavforhue.useWaveForm) {
    // -- Waveform -----------------------------------------------------
    Vertex_t  verts[512];

#ifndef HAS_OPENGL
    /*
    unsigned stride = sizeof(Vertex_t), offset = 0;
    g_context->IASetVertexBuffers(0, 1, &g_vBuffer, &stride, &offset);
    g_context->IASetInputLayout(g_inputLayout);
    g_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    g_context->VSSetShader(g_vShader, 0, 0);
    g_context->VSSetConstantBuffers(0, 1, &g_cViewPort);
    g_context->PSSetShader(g_pShader, 0, 0);
    float xcolor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    */
    g_device->SetFVF(VERTEX_FORMAT);
    g_device->SetPixelShader(NULL);
#endif

    // Left (upper) channel
#ifdef HAS_OPENGL
    GLenum errcode;
    //glColor3f(1.0, 1.0, 1.0);
    glColor3f(wt.wavforhue.rgb[0], wt.wavforhue.rgb[1], wt.wavforhue.rgb[2]);
    glDisable(GL_BLEND);
    glPushMatrix();
    glTranslatef(0, 0, -1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < wt.wavforhue.iMaxAudioData_i; i++)
#else
    for (int i = 0; i < 256; i++)
#endif
    {
#ifdef HAS_OPENGL
      //verts[i].col = 0xffffffff;
      //verts[i].x = g_viewport.TopLeftX + ((i / wt.wavforhue.fMaxAudioData) * g_viewport.Width);
      verts[i].x = g_viewport.X + ((i / wt.wavforhue.fMaxAudioData) * g_viewport.Width);
#else
      //verts[i].col = XMFLOAT4(xcolor);
      //verts[i].col = XMFLOAT4(wt.wavforhue.rgb[0], wt.wavforhue.rgb[1], wt.wavforhue.rgb[2], 1.0f);
      verts[i].col = D3DCOLOR_COLORVALUE(wt.wavforhue.rgb[0], wt.wavforhue.rgb[1], wt.wavforhue.rgb[2], 1.0f);
      //verts[i].x = g_viewport.TopLeftX + ((i / 255.0f) * g_viewport.Width);
      verts[i].x = g_viewport.X + ((i / 255.0f) * g_viewport.Width);
#endif
      //verts[i].y = g_viewport.TopLeftY + g_viewport.Height * 0.33f
      //  + (g_fWaveform[0][i] * g_viewport.Height * 0.15f);
      verts[i].y = g_viewport.Y + g_viewport.Height * 0.33f + (g_fWaveform[0][i] * g_viewport.Height * 0.15f);
      verts[i].z = 1.0;
#ifdef HAS_OPENGL
      glVertex2f(verts[i].x, verts[i].y);
#endif
    }
#ifdef HAS_OPENGL
    glEnd();
    if ((errcode = glGetError()) != GL_NO_ERROR) {
      printf("Houston, we have a GL problem: %s\n", gluErrorString(errcode));
    }
//#endif
#elif !defined(HAS_OPENGL)
    g_device->DrawPrimitiveUP(D3DPT_LINESTRIP, wt.wavforhue.iMaxAudioData_i - 1, verts, sizeof(Vertex_t));
#endif

    // Right (lower) channel
#ifdef HAS_OPENGL
    glBegin(GL_LINE_STRIP);
    // WavforHue mod
    for (int i = 0; i < wt.wavforhue.iMaxAudioData_i; i++)
#else
    //for (int i = 256; i < 512; i++)
    for (int i = 0; i < 256; i++)
#endif
    {
#ifdef HAS_OPENGL
      //verts[i].col = 0xffffffff;
      //verts[i].x = g_viewport.TopLeftX + ((i / wt.wavforhue.fMaxAudioData) * g_viewport.Width);
      verts[i].x = g_viewport.X + ((i / wt.wavforhue.fMaxAudioData) * g_viewport.Width);
#else
      //verts[i].col = XMFLOAT4(xcolor);
      //verts[i].col = XMFLOAT4(wt.wavforhue.rgb[0], wt.wavforhue.rgb[1], wt.wavforhue.rgb[2], 1.0f);
      verts[i].col = D3DCOLOR_COLORVALUE(wt.wavforhue.rgb[0], wt.wavforhue.rgb[1], wt.wavforhue.rgb[2], 1.0f);
      //verts[i].x = g_viewport.TopLeftX + (((i - 256) / 255.0f) * g_viewport.Width);
      verts[i].x = g_viewport.X + ((i / 255.0f) * g_viewport.Width);
#endif
      //verts[i].y = g_viewport.TopLeftY + g_viewport.Height * 0.66f
      //  + (g_fWaveform[1][i] * g_viewport.Height * 0.15f);
      verts[i].y = g_viewport.Y + g_viewport.Height * 0.66f
        + (g_fWaveform[1][i] * g_viewport.Height * 0.15f);
      verts[i].z = 1.0;
#ifdef HAS_OPENGL
      glVertex2f(verts[i].x, verts[i].y);
#endif
    }

#ifdef HAS_OPENGL
    glEnd();
    glEnable(GL_BLEND);
    glPopMatrix();
    if ((errcode = glGetError()) != GL_NO_ERROR) {
      printf("Houston, we have a GL problem: %s\n", gluErrorString(errcode));
    }
#elif !defined(HAS_OPENGL)
    /*
    // a little optimization: generate and send all vertecies for both channels
    D3D11_MAPPED_SUBRESOURCE res;
    if (S_OK == g_context->Map(g_vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
    {
      memcpy(res.pData, verts, sizeof(Vertex_t) * 512);
      g_context->Unmap(g_vBuffer, 0);
    }
    // draw left channel
    g_context->Draw(256, 0);
    // draw right channel
    g_context->Draw(256, 256);
    */
    g_device->DrawPrimitiveUP(D3DPT_LINESTRIP, wt.wavforhue.iMaxAudioData_i - 1, verts, sizeof(Vertex_t));
#endif
    // -- Waveform -----------------------------------------------------
  }

  // -- WavforHue function calls -------------------------------------
  //get some interesting numbers to play with
  wt.wavforhue.UpdateTime();
  wt.wavforhue.timePass = wt.wavforhue.fElapsedAppTime;
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
#include "WavforHue_Settings.hpp"


//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
extern "C" void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}

/*
// -- Waveform -----------------------------------------------------
#ifndef HAS_OPENGL
bool init_renderer_objs()
{
  // Create vertex shader
  if (S_OK != g_device->CreateVertexShader(DefaultVertexShaderCode, sizeof(DefaultVertexShaderCode), nullptr, &g_vShader))
    return false;

  // Create input layout
  D3D11_INPUT_ELEMENT_DESC layout[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };
  if (S_OK != g_device->CreateInputLayout(layout, ARRAYSIZE(layout), DefaultVertexShaderCode, sizeof(DefaultVertexShaderCode), &g_inputLayout))
    return false;

  // Create pixel shader
  if (S_OK != g_device->CreatePixelShader(DefaultPixelShaderCode, sizeof(DefaultPixelShaderCode), nullptr, &g_pShader))
    return false;

  // create buffers
  CD3D11_BUFFER_DESC desc(sizeof(Vertex_t) * 512, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  if (S_OK != g_device->CreateBuffer(&desc, NULL, &g_vBuffer))
    return false;

  desc.ByteWidth = sizeof(cbViewPort);
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.CPUAccessFlags = 0;

  cbViewPort viewPort = { (float)g_viewport.Width, (float)g_viewport.Height, 0.0f, 0.0f };
  D3D11_SUBRESOURCE_DATA initData;
  initData.pSysMem = &viewPort;

  if (S_OK != g_device->CreateBuffer(&desc, &initData, &g_cViewPort))
    return false;

  // we are ready
  return true;
}
#endif // !HAS_OPENGL
// -- Waveform -----------------------------------------------------
*/
