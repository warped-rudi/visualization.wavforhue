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
#define WAVFORHUE_MAIN

#include <cstring>

#ifndef WAVFORHUE_THREAD
#include "WavforHue_Thread.h"
#endif

// -- Waveform -----------------------------------------------------
#ifdef HAS_OPENGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#else
#ifdef _WIN32
//dx11 is not working for me.
/*
#include <d3d11_1.h> 
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
*/
#include <D3D9.h>
#endif
#endif
// -- Waveform -----------------------------------------------------

// Included last to prevent including winsock.h on Windows.
// This happens when windows.h is included before curl.h.
// Did I mention I don't like curl?
#ifndef WAVFORHUE
#include "WavforHue.h"
#endif
#include <libKODI_guilib.h>
#include <xbmc_vis_dll.h> 
#include <libXBMC_addon.h>
#include "platform/util/util.h"
extern ADDON::CHelper_libXBMC_addon *XBMC;

// -- Waveform -----------------------------------------------------
char g_visName[512];
#ifndef HAS_OPENGL
/*
ID3D11Device*             g_device = NULL;
ID3D11DeviceContext*      g_context = NULL;
ID3D11VertexShader*       g_vShader = NULL;
ID3D11PixelShader*        g_pShader = NULL;
ID3D11InputLayout*        g_inputLayout = NULL;
ID3D11Buffer*             g_vBuffer = NULL;
ID3D11Buffer*             g_cViewPort = NULL;

using namespace DirectX;
using namespace DirectX::PackedVector;

// Include the precompiled shader code.
namespace
{
#include "DefaultPixelShader.inc"
#include "DefaultVertexShader.inc"
}

struct cbViewPort
{
  float g_viewPortWidth;
  float g_viewPortHeigh;
  float align1, align2;
};
*/
LPDIRECT3DDEVICE9 g_device;
#else
void* g_device;
#endif
float g_fWaveform[2][512];

#ifdef HAS_OPENGL
/*
typedef struct {
  int TopLeftX;
  int TopLeftY;
  int Width;
  int Height;
  int MinDepth;
  int MaxDepth;
} D3D11_VIEWPORT;
*/
typedef struct {
  int X;
  int Y;
  int Width;
  int Height;
  int MinZ;
  int MaxZ;
} D3DVIEWPORT9;
typedef unsigned long D3DCOLOR;
#endif

//D3D11_VIEWPORT g_viewport;
D3DVIEWPORT9  g_viewport;

struct Vertex_t
{
  float x, y, z;
#ifdef HAS_OPENGL
  D3DCOLOR  col;
#else
  //XMFLOAT4 col;
  D3DCOLOR  col;
#endif
};

#ifndef HAS_OPENGL
#define VERTEX_FORMAT     (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#endif

/*
#ifndef HAS_OPENGL
bool init_renderer_objs();
#endif 
*/
// -- Waveform -----------------------------------------------------

#endif
