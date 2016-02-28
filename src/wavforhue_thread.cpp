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

using namespace ADDON;

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
  SocketData putData;
  std::queue<SocketData> mQueue;
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

void WavforHue_Thread::curlCall(SocketData socketData)
{
#ifndef _WIN32
  CURL *curl;
  CURLcode res;
  std::string url = "http://" + socketData.host + socketData.path;
  curl = curl_easy_init();
  // Now specify we want to PUT data, but not using a file, so it has o be a CUSTOMREQUEST
  curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
  if (url.substr(url.length() - 3) == "api")
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  else
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  // This eliminates all kinds of HTTP responses from showing up in stdin.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WavforHue_Thread::noop_cb);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, socketData.json.c_str());
  // Set the URL that is about to receive our POST. 
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  // Perform the request, res will get the return code
  res = curl_easy_perform(curl);
  // always cleanup curl
  curl_easy_cleanup(curl);
#else
  std::string request, response;
  int resp_leng;
  char buffer[BUFFERSIZE];

  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    XBMC->Log(LOG_DEBUG, "WSAStartup failed.");
    abort();
  }
  SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  struct hostent *host;
  host = gethostbyname(socketData.host.c_str());
  SOCKADDR_IN SockAddr;
  SockAddr.sin_port = htons(80);
  SockAddr.sin_family = AF_INET;
  SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
  XBMC->Log(LOG_DEBUG, "Connecting...");
  if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0){
    XBMC->Log(LOG_DEBUG, "Could not connect.");
    abort();
  }
  XBMC->Log(LOG_DEBUG, "Connected.");

  std::stringstream ss;
  ss << socketData.json.length();

  std::stringstream request2;

  request2 << socketData.method << " " << socketData.path << " HTTP/1.1" << std::endl;
  request2 << "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)" << std::endl;
  //request2 << "" << endl;
  request2 << "Host: " << host << std::endl;
  request2 << "Content-Length: " << socketData.json.length() << std::endl;

  request2 << "Content-Type: application/x-www-form-urlencoded" << std::endl;
  request2 << "Accept-Language: en" << std::endl;
  request2 << std::endl;
  request2 << socketData.json;
  request = request2.str();

  // Send request
  if (send(Socket, request.c_str(), request.length(), 0) != request.length())
    XBMC->Log(LOG_DEBUG, "send() sent a different number of bytes than expected.");

  // Get response
  response = "";
  resp_leng = BUFFERSIZE;
  while (resp_leng == BUFFERSIZE)
  {
    resp_leng = recv(Socket, (char*)&buffer, BUFFERSIZE, 0);
    if (resp_leng>0)
      response += std::string(buffer).substr(0, resp_leng);
    // Note: download lag is not handled in this code
  }

  //disconnect
  closesocket(Socket);

  //cleanup
  WSACleanup();

  XBMC->Log(LOG_DEBUG, response.c_str());
#endif
}

void WavforHue_Thread::transferQueueToMain()
{
	SocketData putData;
	while (!wavforhue.queue.empty())
	{
    putData = wavforhue.queue.front(); wavforhue.queue.pop();
    curlCall(putData);
  }
}

void WavforHue_Thread::transferQueueToThread()
{
  SocketData putData;
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
