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

#ifndef WAVFORHUE_THREAD
#include "wavforhue_thread.h"
#endif


// -- Constructor ----------------------------------------------------
WavforHue_Thread::WavforHue_Thread()
{
}
// -- Constructor ----------------------------------------------------

// -- Destructor ----------------------------------------------------- 
WavforHue_Thread::~WavforHue_Thread()
{
}
// -- Destructor ----------------------------------------------------- 

// -- Threading ----------------------------------------------------------
// This helps hated cURL not output a ton of crap to stdout
size_t WavforHue_Thread::noop_cb(void *ptr, size_t size, size_t nmemb, void *data) {
  return size * nmemb;
}

// This thread keeps cURL from puking all over the waveform, suprising it and
// making it jerk away.
void WavforHue_Thread::workerThread()
{
  bool isEmpty;
  PutData putdata;
  // This thread comes alive when AudioData(), Create() or Start() has put an 
  // item in the stack. It runs until Destroy() or Stop() sets gRunThread to 
  // false and joins it. Or something like that. It's actually magic.
  while (gRunThread)
  {
    //check that an item is on the stack
    {
      std::lock_guard<std::mutex> lock(gMutex);
      isEmpty = gQueue.empty();
    }
    if (isEmpty)
    {
      //Wait until AudioData() sends data.
      std::unique_lock<std::mutex> lock(gMutex);
      gThreadConditionVariable.wait(lock, [&]{return gReady; });
    }
    else
    {
      std::lock_guard<std::mutex> lock(gMutex);
      putdata = gQueue.front();
      gQueue.pop();
    }
    if (!isEmpty)
    {
      CURL *curl;
      CURLcode res;
      curl = curl_easy_init();
      // Now specify we want to PUT data, but not using a file, so it has o be a CUSTOMREQUEST
      curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
      //error: invalid use of non-static member function
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WavforHue_Thread::noop_cb);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, putdata.json.c_str());
      // Set the URL that is about to receive our POST. 
      curl_easy_setopt(curl, CURLOPT_URL, putdata.url.c_str());
      // Perform the request, res will get the return code
      res = curl_easy_perform(curl);
      // always cleanup curl
      curl_easy_cleanup(curl);
    }
  }
}

void WavforHue_Thread::transferQueue()
{
  PutData putData;
  gRunThread = true;
  // Check if the thread is alive yet.
  if (!gWorkerThread.joinable())
  {
    gWorkerThread = std::thread(&WavforHue_Thread::workerThread, this);
  }
  while (wavforhue.queue.size())
  {
    putData = wavforhue.queue.front();
    wavforhue.queue.pop();
    if (gQueue.size() < 10)
    {
      std::lock_guard<std::mutex> lock(gMutex);
      gQueue.push(putData);
    }
  }
  // Let the thread know to start processing.
  {
    std::lock_guard<std::mutex> lock(gMutex);
    gReady = true;
  }
  gThreadConditionVariable.notify_one();
}
//-- Threading -----------------------------------------------------
