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
#define WAVFORHUE_MAIN

#include <xbmc_vis_dll.h>
#include <cstring>

#ifndef WAVFORHUE
#include "wavforhue.h"
#endif

#ifndef WAVFORHUE_THREAD
#include "wavforhue_thread.h"
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
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#endif
#endif
// -- Waveform -----------------------------------------------------


// -- Waveform -----------------------------------------------------
char g_visName[512];
#ifndef HAS_OPENGL
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

#else
void* g_device;
#endif
float g_fWaveform[2][512];

#ifdef HAS_OPENGL
typedef struct {
  int TopLeftX;
  int TopLeftY;
  int Width;
  int Height;
  int MinDepth;
  int MaxDepth;
} D3D11_VIEWPORT;
typedef unsigned long D3DCOLOR;
#endif

D3D11_VIEWPORT g_viewport;

struct Vertex_t
{
  float x, y, z;
#ifdef HAS_OPENGL
  D3DCOLOR  col;
#else
  XMFLOAT4 col;
#endif
};

#ifndef HAS_OPENGL
bool init_renderer_objs();
#endif 
// -- Waveform -----------------------------------------------------

#endif
