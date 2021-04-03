/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include<vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <vdr/interface.h>
#include <vdr/tools.h>
#include "global.h"
#include "tcpsvr.h"
#include "telnet.h"


/*******************************************************************************
 * cTcpServer
 ******************************************************************************/

cTcpServer::cTcpServer(int Port, const char* name) :
  cThread(name), mPort(Port), mSockfd(-1), mActive(true)
{
  mName = name;
  debug_plugin("'%s' (%p)", mName.c_str(), this);
}

cTcpServer::~cTcpServer() {
  debug_plugin("'%s' (%p)", mName.c_str(), this);
  Stop();
}

void cTcpServer::Stop() {
  debug_plugin("%p", this);
  Close();
  mActive = false;
  mWait.Signal();
}

bool cTcpServer::Open() {
  if (mSockfd > 0)
     return true;
  int opt;

  /* create a STREAM (TCP) socket in the INET6 (IPv6) protocol */
  mSockfd = socket(PF_INET6, SOCK_STREAM, 0);
  if (mSockfd < 0) {
     esyslog("%s: could not create socket: %m", __PRETTY_FUNCTION__);
     return false;
     }

  /* let socket accept v6 and v4 (mapped address) connections */
  opt = 0;
  if (setsockopt(mSockfd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) < 0) {
     esyslog("%s: could not turn off IPV6_V6ONLY: %m", __PRETTY_FUNCTION__);
     return false;
     }

  /* allow socket to always reuse the same port */
  opt = 1;
  if (setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
     esyslog("%s: could not turn on SO_REUSEADDR: %m", __PRETTY_FUNCTION__);

  /* bind socket to any address on port 'mPort' */
  struct sockaddr_in6 server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_addr = in6addr_any;
  server_addr.sin6_port = htons(mPort);

  if (bind(mSockfd, (struct sockaddr*) &server_addr,  sizeof(server_addr)) < 0) {
     esyslog("%s: couldnt bind socket to port %d: %m", __PRETTY_FUNCTION__, mPort);
     Close();
     return false;
     }

  /* switch socket to non-blocking */
  opt = fcntl(mSockfd, F_GETFL, 0);
  if (opt < 0) {
     esyslog("%s: couldnt get socket flags: %m", __PRETTY_FUNCTION__);
     Close();
     return false;
     }
  opt |= O_NONBLOCK;
  if (fcntl(mSockfd, F_SETFL, opt) < 0) {
     esyslog("%s: couldnt set socket flags: %m", __PRETTY_FUNCTION__);
     Close();
     return false;
     }

  /* start listening on socket */
  if (listen(mSockfd, 5) < 0) {
     esyslog("%s: couldnt start listening: %m", __PRETTY_FUNCTION__);
     Close();
     return false;
     }
  return true;
}


void cTcpServer::Close() {
  mActive = false;
  if (mSockfd >= 0) {
     close(mSockfd);
     mSockfd = -1;
     }
}


int cTcpServer::Accept() {
  if (not(Open())) {
     esyslog("%s: Open() failed", __PRETTY_FUNCTION__);
     return -1;
     }

  char addrbuf[INET6_ADDRSTRLEN];
  struct sockaddr_in6 client_addr;
  socklen_t len = sizeof(client_addr);
  int client = accept(mSockfd, (struct sockaddr*) &client_addr, &len);

  if (client < 0) {
     if (errno != EINTR && errno != EAGAIN) {
        esyslog("%s: accept failed: %m", __PRETTY_FUNCTION__);
        return -2;
        }
     return -1;
     }
  /* as this is an IPv6 socket, it reports IPv4 addresses as
   * 'IPv4-mapped IPv6 address' with prefix '::ffff:', ie.
   *  '::ffff:127.0.0.1' for '127.0.0.1'
   */
  isyslog("connect from %s port %d",
         inet_ntop(AF_INET6, &client_addr.sin6_addr, addrbuf, INET6_ADDRSTRLEN),
         mPort);
  return client;
}

void cTcpServer::Action() {
  std::vector<cCtrlGateway*> clients;
  int fd;

  if (! mPort || ! Open()) {
     esyslog("control: could not open telnet port '%d'.", mPort);
     return;
     }

  while(mActive && Running()) {
     mWait.Wait(100);
     if ((fd = Accept()) < 0)
        continue;

     char buf[256];
     sprintf(buf, "%s::client(%d)", mName.c_str(), fd);
     cCtrlGateway* client = new cCtrlGateway(fd, buf);
     if (client == NULL) {
        esyslog("out of memory");
        break;
        }

     client->Start();
     clients.push_back(client);
     if (Interface)
        Interface->LearnKeys();
     }

  // destroy all gateway objects and clear it's list.
  for(auto gw = clients.begin(); gw != clients.end(); ++gw) {
     DELETENULL(*gw);
     }
  clients.clear();

  Close();
  Cancel(0);
}
