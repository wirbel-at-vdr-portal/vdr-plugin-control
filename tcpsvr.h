/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _tcpsvr_h_
#define _tcpsvr_h_

#include <vdr/thread.h>

class cTcpServer : public cThread {
private:
  std::string mName;
  int mPort;
  int mSockfd;
  cCondWait mWait;
  volatile bool mActive;
  virtual void Action();
  int Accept();
  bool Open();
  void Close();
public:
  cTcpServer(int Port, const char* name);
  ~cTcpServer();
  void Stop();
};

#endif
