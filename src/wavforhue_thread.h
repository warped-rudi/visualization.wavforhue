/*
*      Copyright (C) 2015-2016 Thomas M. Hardy
*      Copyright (C) 2005-2016 Team Kodi
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

#ifndef WAVFORHUE_THREAD
#define WAVEFORHUE_THREAD

// -- cURL works in *nix, but is crap in Windows -------------------
#ifndef _WIN32
#include <curl/curl.h>
#else
// -- cURL works in *nix, but is crap in Windows -------------------
#define _WIN32_WINNT 0x0501
#include <Winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment (lib, "Ws2_32.lib")
#endif
// -- cURL works in *nix, but is crap in Windows -------------------

// -- trim ---------------------------------------------------------
#include <cctype>
// -- trim ---------------------------------------------------------

// -- Threading ----------------------------------------------------
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
// -- Threading ----------------------------------------------------

// -- Logging ------------------------------------------------------
#include <libXBMC_addon.h>
// -- Logging ------------------------------------------------------


#ifndef WAVFORHUE
#include "WavforHue.h"
#endif

// -- Threading ----------------------------------------------------
class WavforHue_Thread
{
public:
  WavforHue wavforhue;
  WavforHue_Thread();
  ~WavforHue_Thread();

  // -- Logging ------------------------------------------------------
  ADDON::CHelper_libXBMC_addon *XBMC;
  // -- Logging ------------------------------------------------------

  void transferQueueToThread();
  void transferQueueToMain();
  void stop();

private:
  std::string response;
  std::thread gWorkerThread;
  std::mutex gMutex;
  std::condition_variable gThreadConditionVariable;
  std::atomic<bool> gRunThread;
  bool gReady;
  std::queue<SocketData> gQueue;

  static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
  void workerThread();
  void httpRequest(SocketData putData);
};



// -- Threading ----------------------------------------------------

#endif
