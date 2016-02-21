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

#ifndef WAVFORHUE
#include "wavforhue.h"
#endif

#ifndef WAVFORHUE_THREAD
#define WAVEFORHUE_THREAD

// -- cURL -- I hate cURL ------------------------------------------
// -----------------------------------------------------------------
// This is a workaround so I can build and test on my f'ed up 
// Windows 7 box. Take this out if you are not me!! It will break
// curl in Kodi if left here.
#ifdef _WIN32
#include <WinSock2.h>
#endif
// -----------------------------------------------------------------
#include <curl/curl.h>
// -- cURL -- I hate cURL ------------------------------------------

// -- Threading ----------------------------------------------------
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
// -- Threading ----------------------------------------------------


// -- Threading ----------------------------------------------------
class WavforHue_Thread
{
public:
  std::thread gWorkerThread;
  std::mutex gMutex;
  std::condition_variable gThreadConditionVariable;
  std::atomic<bool> gRunThread;
  bool gReady;
  std::queue<PutData> gQueue;
  WavforHue wavforhue;
  WavforHue_Thread();
  ~WavforHue_Thread();

  size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data);
  void workerThread();
  void transferQueue();
};



// -- Threading ----------------------------------------------------

#endif
