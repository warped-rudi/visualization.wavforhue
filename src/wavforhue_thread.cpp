/*
*      Copyright (C) 2015-2016 Thomas M. Hardy
*      Copyright (C) 2003-2016 Team Kodi
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
#include "WavforHue_Thread.h"
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
  PutData putData;
  std::queue<PutData> mQueue;
  // This thread comes alive when AudioData(), Create() or Start() has put an 
  // item in the stack. It runs until Destroy() or Stop() sets gRunThread to 
  // false and joins it. Or something like that. It's actually magic.
  while (gRunThread || !mQueue.empty())
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
      // Get everything off the global queue for local processing
      std::lock_guard<std::mutex> lock(gMutex);
      while (!gQueue.empty())
      {
        mQueue.push(gQueue.front());
        gQueue.pop();
      }
    }
    while (!mQueue.empty())
    {
      putData = mQueue.front(); mQueue.pop();
      curlCall(putData);
    }
  }
}

void WavforHue_Thread::curlCall(PutData putData)
{
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  // Now specify we want to PUT data, but not using a file, so it has o be a CUSTOMREQUEST
  curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
  if(putData.url.substr(putData.url.length() - 3) == "api")
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  else
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  // This eliminates all kinds of HTTP responses from showing up in stdin.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WavforHue_Thread::noop_cb);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, putData.json.c_str());
  // Set the URL that is about to receive our POST. 
  curl_easy_setopt(curl, CURLOPT_URL, putData.url.c_str());
  // Perform the request, res will get the return code
  res = curl_easy_perform(curl);
  // always cleanup curl
  curl_easy_cleanup(curl);
}

void WavforHue_Thread::transferQueueToMain()
{
	PutData putData;
	while (!wavforhue.queue.empty())
	{
    putData = wavforhue.queue.front(); wavforhue.queue.pop();
    curlCall(putData);
  }
}

void WavforHue_Thread::transferQueueToThread()
{
  PutData putData;
  gRunThread = true;
  // Check if the thread is alive yet.
  if (!gWorkerThread.joinable())
  {
    gWorkerThread = std::thread(&WavforHue_Thread::workerThread, this);
  }
  while (!wavforhue.queue.empty())
  {
    putData = wavforhue.queue.front();
    wavforhue.queue.pop();
    //if (gQueue.size() < 10)
    //{
      std::lock_guard<std::mutex> lock(gMutex);
      gQueue.push(putData);
    //}
  }
  // Let the thread know to start processing.
  {
    std::lock_guard<std::mutex> lock(gMutex);
    gReady = true;
  }
  gThreadConditionVariable.notify_one();
}
//-- Threading -----------------------------------------------------
