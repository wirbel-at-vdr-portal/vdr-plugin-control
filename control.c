/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>
#include "global.h"
#include "tcpsvr.h"
#include "telnet.h"



/*******************************************************************************
 * cPluginControl
 ******************************************************************************/

/*******************************************************************************
 * reasonable versioning rules:
 *   - MAJOR++  -> API-incompatible
 *   - MINOR++  -> API-compatible, but new functionality
 *   - PATCH++  -> API untouched changes
 ******************************************************************************/

static const char *VERSION        = "1.0.1";
static const char *DESCRIPTION    = "telnet remote control";
static const char *MAINMENUENTRY  = NULL;

class cPluginControl : public cPlugin {
private:
  int mPort;
  std::string mTTYName;
  cTcpServer* mTCP;
  cCtrlGateway* mTTY;
public:
  cPluginControl(void);
  virtual ~cPluginControl();
  virtual const char* Version(void) { return VERSION; }
  virtual const char* Description(void) { return DESCRIPTION; }
  virtual const char* CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char* argv[]);
  virtual bool Initialize(void) { return true; };
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void) {};                           /* No housekeeping, we're always clean. */
//virtual void MainThreadHook(void);                            /* No vdr main thread hook. */
//virtual cString Active(void);                                 /* Not blocking shutdown. */
//virtual time_t WakeupTime(void);                              /* No wake up. */
  virtual const char* MainMenuEntry(void)   { return MAINMENUENTRY; }
  virtual cOsdObject* MainMenuAction(void)  { return NULL; };
  virtual cMenuSetupPage* SetupMenu(void)   { return NULL; };
  virtual bool SetupParse(const char* Name, const char* Value) { return false; };
//virtual bool Service(const char *Id, void *Data = NULL);      /* No service Interface. */
//virtual const char **SVDRPHelpPages(void);                    /* No SVDRP support, no SVDRP help. */
//virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode); /* no SVDRP. */
};




/*******************************************************************************
 * cPluginControl - implementation
 ******************************************************************************/
cPluginControl::cPluginControl(void) : mPort(2002), mTCP(NULL), mTTY(NULL)
{}

cPluginControl::~cPluginControl() {
  debug_plugin("---------------------------------------------");
  DELETENULL(mTCP);
  DELETENULL(mTTY);
}

void cPluginControl::Stop(void) {
  debug_plugin("---------------------------------------------");
  if (mTTY)
     mTTY->Close();
  if (mTCP)
     mTCP->Stop();
}


const char *cPluginControl::CommandLineHelp(void) {
  const char* help =
"  -t TTY,   --terminal=TTY tty for local control\n"
"                           (default: none)\n"
"  -p PORT,  --port=PORT    telnet port for incoming connections\n"
"                           (default: 2002)\n";
  return help;
}

bool cPluginControl::ProcessArgs(int argc, char* argv[]) {
  int c, i;
  const struct option l[] = {
     { "terminal", required_argument, NULL, 't' },
     { "port",     required_argument, NULL, 'p' },
     { NULL,       no_argument,       NULL,  0  },
     };

  while((c = getopt_long(argc, argv, "t:p:", l, &i)) != -1) {
     switch(c) {
        case 't':
           mTTYName = optarg;
           break;
        case 'p':
           mPort = atoi(optarg);
           break;
        default:
           return false;
        }
     }
  return true;
}

bool cPluginControl::Start(void) {
  if (mPort) {
     mTCP = new cTcpServer(mPort, "control::mTCP");
     if (mTCP == NULL) {
        esyslog("%s: out of memory\n", __PRETTY_FUNCTION__);
        return false;
        }
     debug_plugin("mTCP->Start()");
     mTCP->Start();
     }

  if (mTTYName.size()) {
     int fd = open(mTTYName.c_str(), O_RDWR);
     if (fd < 0) {
        esyslog("%s: cannot open tty %s: %m", __PRETTY_FUNCTION__, mTTYName.c_str());
        return false;
        }
     mTTY = new cCtrlGateway(fd, "control::mTTY");
     if (mTTY == NULL) {
        esyslog("%s: out of memory\n", __PRETTY_FUNCTION__);
        return false;
        }
     mTTY->Start();
     }
  return true;
}


VDRPLUGINCREATOR(cPluginControl); // Don't touch this!
