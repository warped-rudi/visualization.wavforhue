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

#ifndef WAVFORHUE_GLES_MAIN
#define WAVFORHUE_GLES_MAIN

#include <xbmc_vis_dll.h>
#include <cstring>

#ifndef WAVFORHUE
#include "wavforhue.h"
#endif

#ifndef WAVFORHUE_THREAD
#include "wavforhue_thread.h"
#endif

// -- Waveform -----------------------------------------------------
#if defined(__APPLE__)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "VisGUIShader.h"
// -- Waveform -----------------------------------------------------




// -- Waveform -----------------------------------------------------
#define NUM_BANDS 16
#ifndef M_PI
#define M_PI       3.141592654f
#endif
#define DEG2RAD(d) ( (d) * M_PI/180.0f )

const char *frag = "precision mediump float; \n"
                   "varying lowp vec4 m_colour; \n"
                   "void main () \n"
                   "{ \n"
                   "  gl_FragColor = m_colour; \n"
                   "}\n";

const char *vert = "attribute vec4 m_attrpos;\n"
                   "attribute vec4 m_attrcol;\n"
                   "attribute vec4 m_attrcord0;\n"
                   "attribute vec4 m_attrcord1;\n"
                   "varying vec4   m_cord0;\n"
                   "varying vec4   m_cord1;\n"
                   "varying lowp   vec4 m_colour;\n"
                   "uniform mat4   m_proj;\n"
                   "uniform mat4   m_model;\n"
                   "void main ()\n"
                   "{\n"
                   "  mat4 mvp    = m_proj * m_model;\n"
                   "  gl_Position = mvp * m_attrpos;\n"
                   "  m_colour    = m_attrcol;\n"
                   "  m_cord0     = m_attrcord0;\n"
                   "  m_cord1     = m_attrcord1;\n"
                   "}\n";

CVisGUIShader  *vis_shader = NULL;
float g_fWaveform[2][512];
// -- Waveform -----------------------------------------------------

#endif