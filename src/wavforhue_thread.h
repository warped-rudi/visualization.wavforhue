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

// -- cURL -- I hate cURL ------------------------------------------
#include <curl/curl.h>
//#include "../../xbmc/xbmc/filesystem/DllLibCurl.h"
// -- cURL -- I hate cURL ------------------------------------------

// -- Threading ----------------------------------------------------
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
// -- Threading ----------------------------------------------------

// Included last to prevent including winsock.h on Windows.
// This happens when windows.h is included before curl.h.
// Did I mention I don't like curl?
#ifndef WAVFORHUE
#include "WavforHue.h"
#endif

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
  void curlCall(PutData putData);
  void transferQueueToThread();
  void transferQueueToMain();
};



// -- Threading ----------------------------------------------------

#endif
