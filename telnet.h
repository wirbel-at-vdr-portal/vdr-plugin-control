/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _CTRL_TELNET_
#define _CTRL_TELNET_

#include <vdr/thread.h>
#include "circbuf.h"
#include "telnetopts.h"



/*******************************************************************************
 * cCtrlGateway
 ******************************************************************************/

class cCtrlGateway : public cThread {
private:
  std::string mName;

  //socket related
  int mSocket;
  unsigned char mSocketBuf[256];
  cCircularBuffer<unsigned char, 32768> mBuffer;

  void socket_Close();
  bool socket_Poll(int msec);
  int  socket_Receive(unsigned char* data, int size);
  int  socket_Send(const unsigned char* data, int size);


  //telnet related
  int  mTelnetState;
  int  mTelnetWidth;
  int  mTelnetHeight;
  unsigned char  mSubCmd[256];
  unsigned char* mSubCmdPos;
  unsigned char* mSubCmdEnd;
  struct {
     unsigned char option;
     int pos;
     int state;
     } mOptions[MAXTELNETOPTS];

  void telnet_Init();
  int  telnet_Receive(const unsigned char* data, int size);
  int  telnet_IAC(unsigned char code, unsigned char negotation);

  void OnCmdWill(unsigned char c);
  void OnCmdWont(unsigned char c);
  void OnCmdDo(unsigned char c);
  void OnCmdDont(unsigned char c);
  void OnSubCmd();
  inline void PutChar(unsigned char c);
  inline void AccuSubCmd(unsigned char c);
  bool SetOption(unsigned char Option, int local, int State);
  int  GetOption(unsigned char Option, int local);

  volatile bool    mActive;

  virtual void Action();
public:
  cCtrlGateway(int socket, const char* name);
  ~cCtrlGateway();
  bool SendMsg(const char* Msg);
  void Close();
};


#endif
