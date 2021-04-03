/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <malloc.h>
#include <string.h>
#include <poll.h>
#include <vdr/remote.h>
#include "global.h"
#include "telnet.h"
#include "keyboard.h"
#include "osdstate.h"
#include "term.h"

#define debug_telnet(s...)

static const unsigned char goodbye[] = EC_HOME EC_CLRSCR EC_SHOWCURSOR "VDR control plugin: goodbye!\n\r";
static const unsigned char greeting[] = EC_HIDECURSOR "VDR control plugin\n\r";
static const char* iamhere = "\n\r[vdr : yes]\n\r";

#define cBREAK  3
#define cEOF   26
#define cESC   27

cCtrlGateway::cCtrlGateway(int socket, const char* name) :
  cThread(NULL), mSocket(socket),
  mTelnetState(sTelnetNormal),
  mTelnetWidth(80), mTelnetHeight(24),
  mSubCmdPos(mSubCmd), mSubCmdEnd(mSubCmd + 255),
  mActive(false)
{
  mName = name;
  debug_plugin("'%s' %p", name, this);

  // Initialize all implemented telnet options
  mOptions[0] = {OPTION_ECHO, mLocal , sPending};
  mOptions[1] = {OPTION_SGA , mLocal , sPending};
  mOptions[2] = {OPTION_SGA , mRemote, sPending};
  mOptions[3] = {OPTION_NAWS, mRemote, sPending};
}

cCtrlGateway::~cCtrlGateway() {
  debug_plugin("'%s' %p", mName.c_str(), this);
  mActive = false;
  Cancel(20);
  Close();
  socket_Close();
}

void cCtrlGateway::socket_Close() {
  if (mSocket) {
     close(mSocket);
     mSocket = 0;
     }
}

bool cCtrlGateway::socket_Poll(int msec) {
  if (mSocket) {
     pollfd pfd = {.fd = mSocket, .events = POLLIN, .revents = 0};
     return (poll(&pfd, 1, msec) != 0);
     }
  return false;
}

int cCtrlGateway::socket_Receive(unsigned char* data, int size) {
  debug_plugin("size = %d", size);

  // Let's look if there is something in the buffer.
  int nbytes = mBuffer.Read(data, size);
  if (nbytes != 0) {
     hexdump("mBuffer.Read(data, size)", data, nbytes);
     return nbytes;
     }

  // socket read it data and pushes it to mBuffer.
  nbytes = read(mSocket, mSocketBuf, size);

  if (nbytes < 0) {
     esyslog("%s: socket read error: %m", __PRETTY_FUNCTION__);
     return nbytes;
     }
  else if (nbytes == 0) {
     debug_plugin("socket %d: EOF", mSocket);
     return -1;
     }

  telnet_Receive(mSocketBuf, nbytes);
  debug_plugin("retry reading from buffer");
  return mBuffer.Read(data, size);
}


/*****/
void cCtrlGateway::telnet_Init() {
  debug_telnet("protocol stack is up - negotiating options");
  for(int i = 0; i < MAXTELNETOPTS; ++i) {
     if (mOptions[i].state == sPending) {
        mOptions[i].state = sRequested;
        if (mOptions[i].pos == mLocal) {
           debug_telnet("cCtrlTelnet::start: sending WILL (%d)", mOptions[i].option);
           telnet_IAC(TELNET_WILL, mOptions[i].option);
           }
        else {
           debug_telnet("cCtrlTelnet::start: sending DO (%d)", mOptions[i].option);
           telnet_IAC(TELNET_DO, mOptions[i].option);
           }
        }
     }
}


int cCtrlGateway::telnet_Receive(const unsigned char* data, int size) {
  hexdump(__PRETTY_FUNCTION__, data, size);
  for(int i = 0; i < size; ++i) {
     switch(mTelnetState) {
        case sTelnetNormal:
           switch(*data) {
              case TELNET_IAC :
                 mTelnetState = sTelnetInCommand;
                 break;
              case ((char) 8):
                 PutChar(((char) 0x7f));  // Translate to backspace
                 break;
              case 0xD:
                 PutChar(*data);
                 mTelnetState = sTelnetReturn; // Translate different return types
                 break;
              default:
                 PutChar(*data);
              }
           break;
        case sTelnetInCommand:
           switch(*data) {
              case TELNET_IAC: // Repeated IAC -> output one
                 PutChar(*data);
                 mTelnetState = sTelnetNormal;
                 break;
              case TELNET_SB:
                 mTelnetState = sTelnetInSubCommand;
                 break;
              case TELNET_AYT: // Are You There
                 mBuffer.Write((const unsigned char*) iamhere, strlen(iamhere));//OnAreYouThere();
                 mTelnetState = sTelnetNormal;
                 break;
              //--- Options
              case TELNET_WILL:
                 mTelnetState = sTelnetWill;
                 break;
              case TELNET_WONT:
                 mTelnetState = sTelnetWont;
                 break;
              case TELNET_DO:
                 mTelnetState = sTelnetDo;
                 break;
              case TELNET_DONT:
                 mTelnetState = sTelnetDont;
                 break;
              default: //--- Unknown command, ignore
                 mTelnetState = sTelnetNormal;
              }
           break; //sTelnetInCommand
        case sTelnetWill:
           OnCmdWill(*data);
           mTelnetState = sTelnetNormal;
           break;
        case sTelnetWont:
           OnCmdWont(*data);
           mTelnetState = sTelnetNormal;
           break;
        case sTelnetDo:
           OnCmdDo(*data);
           mTelnetState = sTelnetNormal;
           break;
        case sTelnetDont:
           OnCmdDont(*data);
           mTelnetState = sTelnetNormal;
           break;
        case sTelnetInSubCommand:
           switch(*data) {
              case TELNET_IAC:
                 mTelnetState = sTelnetEndSubCommand;
                 break;
              default:
                 AccuSubCmd(*data);
              }
           break;

        case sTelnetEndSubCommand:
           switch(*data) {
              case TELNET_IAC:
                 AccuSubCmd(*data);
                 mTelnetState = sTelnetInSubCommand;
                 break;
              case TELNET_SE:
                 OnSubCmd();
                 mTelnetState = sTelnetNormal;
                 break;
              default:
                 mSubCmdPos = mSubCmd;
                 mTelnetState = sTelnetNormal;
              }
           break;

        case sTelnetReturn:
           mTelnetState = sTelnetNormal;
           switch(*data) {
              case 0x0:
              case 0xA:
                 break;
              default:
                 telnet_Receive(data, 1);
              }
           break;

        default: // Unknown state???
           mTelnetState = sTelnetNormal;
        }
     ++data;
     }
  return 0;
}

int cCtrlGateway::socket_Send(const unsigned char* data, int size) {
  if (! mSocket)
     return -1;
  return write(mSocket, data, size);
}

int cCtrlGateway::telnet_IAC(unsigned char code, unsigned char negotation) {
  unsigned char buf[3] = {TELNET_IAC, code, negotation};
  return socket_Send(buf, sizeof(buf));
}

void cCtrlGateway::OnCmdWill(unsigned char c) {
  debug_telnet("%d", c);
  int state = GetOption(c, mRemote);
  switch(state) {
     case sNotSupported:
        debug_telnet(" DONT");
        telnet_IAC(TELNET_DONT, c);
        break;
     case sNotNegotiated:
     case sPending:
     case sRejected:
        debug_telnet(" AGREED");
        telnet_IAC(TELNET_DO, c);
        __attribute__ ((fallthrough)); // <<<< NOTE: IS HERE A BREAK MISSING ? presumably not.
     case sRequested:
        SetOption(c, mRemote, sAgreed);
        break;
     case sAgreed:
        debug_telnet(" IGNORED");
        break;
     default:
        debug_telnet(" OOPS");
     }
}


void cCtrlGateway::OnCmdWont(unsigned char c) {
  debug_telnet("c = %d", c);
  int state = GetOption(c, mRemote);
  switch(state) {
     case sNotSupported:
        debug_telnet(" REJECTED");
        telnet_IAC(TELNET_DONT, c);
        break;
     case sNotNegotiated:
     case sPending:
     case sAgreed:
        debug_telnet(" REJECTED");
        telnet_IAC(TELNET_DONT, c);  // <<<< NOTE: IS HERE A BREAK MISSING ? presumably not.
        __attribute__ ((fallthrough));
     case sRequested:
        SetOption(c, mRemote, sRejected);
        break;
     case sRejected:
        debug_telnet(" IGNORED");
        break;
     default:
        debug_telnet(" OOPS");
     }
}


void cCtrlGateway::OnCmdDo(unsigned char c) {
  debug_telnet("c = %d", c);
  int state = GetOption(c, mLocal);
  switch(state) {
     case sNotSupported:
        debug_telnet(" WONT");
        telnet_IAC(TELNET_WONT, c);
        break;
     case sNotNegotiated:
     case sPending:
     case sRejected:
        debug_telnet(" WILL");
        telnet_IAC(TELNET_WILL, c);
        __attribute__ ((fallthrough));
     case sRequested:
        SetOption(c, mLocal, sAgreed);
        break;
     case sAgreed:
        debug_telnet(" IGNORED");
        break;
     default:
        debug_telnet(" OOPS");
     }
}


void cCtrlGateway::OnCmdDont(unsigned char c) {
  debug_telnet("c = %d", c);
  int state = GetOption(c, mLocal);
  switch(state) {
     case sNotSupported:
        debug_telnet(" WONT");
        telnet_IAC(TELNET_WONT, c);
        break;
     case sNotNegotiated:
     case sPending:
     case sAgreed:
        debug_telnet(" WONT");
        telnet_IAC(TELNET_WONT, c);
        __attribute__ ((fallthrough));
     case sRequested:
        SetOption(c, mLocal, sRejected);
        break;
     case sRejected:
        debug_telnet(" IGNORED");
        break;
     default:
        debug_telnet(" OOPS");
     }
}


void cCtrlGateway::OnSubCmd() {
  switch(*mSubCmd) {
     case OPTION_NAWS:
        if (mSubCmdPos - mSubCmd == 5) { // length check
           short width, height;
           ((unsigned char*) &width)[1]  = mSubCmd[1];
           ((unsigned char*) &width)[0]  = mSubCmd[2];
           ((unsigned char*) &height)[1] = mSubCmd[3];
           ((unsigned char*) &height)[0] = mSubCmd[4];

           // New width or height reported?
           if (width >= 0)
              mTelnetWidth = width;
           if (height >= 0)
              mTelnetHeight = height;

           debug_telnet("NAWS %d, %d", mTelnetWidth, mTelnetHeight);
           }
        else {
           debug_telnet("NAWS rejected: size %ld", mSubCmdPos - mSubCmd);
           }
        break;
     default:
        debug_telnet("*mSubCmd = %d", *mSubCmd);
     }
  mSubCmdPos = mSubCmd;
}


inline void cCtrlGateway::PutChar(unsigned char c) {
  mBuffer.Write(&c, 1);
}


inline void cCtrlGateway::AccuSubCmd(unsigned char c) {
  if (mSubCmdPos < mSubCmdEnd) { // If the buffer is not full
     *mSubCmdPos++ = c;          // accumulate the new char
     }
}


bool cCtrlGateway::SetOption(unsigned char Option, int local, int State) {
  // search for option
  for(int i = 0; i < MAXTELNETOPTS; ++i) {
     if (mOptions[i].option == Option && (mOptions[i].pos == local)) {
        mOptions[i].state = State;
        return true;
        }
     }
  return false; // option not found
}


int cCtrlGateway::GetOption(unsigned char Option, int local) {
  debug_telnet("searching option (%d)", Option);
  for(int i = 0; i < MAXTELNETOPTS; ++i) {
     if (mOptions[i].option == Option && (mOptions[i].pos == local))
        return mOptions[i].state;
     }

  debug_telnet("unknown telnet option %c", Option);
  return sNotSupported;
}


void cCtrlGateway::Action() {
  /* NOTE: DO NOT DESTROY 'kbd', VDR ITSELF DESTROYS IT.
   * Otherwise double free on exit of vdr.
   */
  cCtrlKeyboard* kbd = new cCtrlKeyboard("cCtrlKeyboard");
  cOsdState osdstate(this, "cOsdState");

  debug_plugin("thread started");
  do {
     debug_plugin("starting stack");
     telnet_Init();
     socket_Send(greeting, sizeof(greeting) - 1);

     mActive = true;
     while(mActive && Running()) {
     if (socket_Poll(100)) {
        uint64_t Command = 0;
        uint i = 0;
        cTimeMs timeout;
        timeout.Set(100);

        while(mActive && i < sizeof(Command)) { 
           uchar c;
           int r = 0;
           if (socket_Poll(10)) {
              r = socket_Receive(&c, 1);
              }
           if (r < 0) {
              debug_plugin("r = %d, closing socket.", r);
              socket_Send(goodbye, sizeof(goodbye)-1);
              socket_Close();

              mActive = false;
              break;
              }

           osdstate.SetSize(mTelnetWidth, mTelnetHeight);

           if (r == 1) {
              debug_plugin("key received (%2x, %i)", c, i);

              if (c == 0 || c == cBREAK || c == cEOF) {
                 Close();
                 break;
                 }
              Command <<= 8;
              Command |= c;
              i++;
              }
           else if (r == 0) {
              /* don't know why, but sometimes special keys that start with
               * 0x1B ('ESC') cause a short gap between the 0x1B and the rest
               * of their codes, so we'll need to wait some 100ms to see if
               * there is more coming up - or whether this really is the 'ESC'
               * key (if somebody knows how to clean this up, please let me know):*/
              if (Command == 0x1B && !timeout.TimedOut())
                 continue;

              if (Command) {
                 debug_plugin("key 0x%lX", Command);
                 if (! kbd->Put(Command)) {
                    //debug_plugin("sending the key command failed");
                    int func = kbd->MapCodeToFunc(Command);
                    if (func) {
                       //debug_plugin("trying to send the function key (func=%d, %d)", func, func - 256);
                       if (! kbd->Put((eKeys) KBDKEY(func))) {
                          //debug_plugin("sending the function key failed");
                          }
                       }
                    }
                 } //Command
              break;
              }
           } //while mActive & i < sizeof(Command)
        } // if (socket_Poll(100))
     } // while mActive
  } while(0);

  debug_plugin("gateway thread ended (pid=%d)", getpid());
}

bool cCtrlGateway::SendMsg(const char* Msg) {
  return (socket_Send((const unsigned char*) Msg, strlen(Msg)) >= 0);
}

void cCtrlGateway::Close() {
  socket_Send(goodbye, sizeof(goodbye) - 1);
  socket_Close();
  mActive = false;
}
