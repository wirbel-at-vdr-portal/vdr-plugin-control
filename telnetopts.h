#ifndef _telnetopts_h_
#define _telnetopts_h_

/* useful links:
 * - https://tools.ietf.org/html/rfc854
 * - http://www.cs.cf.ac.uk/Dave/Internet/node141.html
 */




/*******************************************************************************
 * Telnet options
 ******************************************************************************/
#define OPTION_ECHO        ((unsigned char) 1)     // Echo
#define OPTION_SGA         ((unsigned char) 3)     // Suppress Go Ahead
#define OPTION_STATUS      ((unsigned char) 5)     // Status
#define OPTION_TIMING_MARK ((unsigned char) 6)     // Timing Mark 
#define OPTION_TT          ((unsigned char) 24)    // Terminal Type
#define OPTION_NAWS        ((unsigned char) 31)    // Window Size
#define OPTION_SPEED       ((unsigned char) 32)    // Terminal Speed
#define OPTION_RFC         ((unsigned char) 33)    // remote flow control
#define OPTION_LMODE       ((unsigned char) 34)    // linemode
#define OPTION_ENV         ((unsigned char) 36)    // environment variables

#define MAXTELNETOPTS 4 /* 20 */


/*******************************************************************************
 * Telnet control commands
 ******************************************************************************/

#define TELNET_SE    ((unsigned char) 240)   // End of subnegotiation parameters.

#define TELNET_NOP   ((unsigned char) 241)   // No operation.

#define TELNET_DM    ((unsigned char) 242)   // Data Mark: The data stream portion of a Sync.
                                             // This should always be accompanied
                                             // by a TCP Urgent notification.

#define TELNET_BREAK ((unsigned char) 243)   // Break: NVT character BRK.

#define TELNET_IP    ((unsigned char) 244)   // Interrupt Process

#define TELNET_AO    ((unsigned char) 245)   // Abort output

#define TELNET_AYT   ((unsigned char) 246)   // Are You There

#define TELNET_EC    ((unsigned char) 247)   // Erase character

#define TELNET_EL    ((unsigned char) 248)   // Erase Line

#define TELNET_GA    ((unsigned char) 249)   // Go ahead

#define TELNET_SB    ((unsigned char) 250)   // Indicates that what follows is
                                             // subnegotiation of the indicated option.

#define TELNET_WILL  ((unsigned char) 251)   // Indicates the desire to begin performing, or confirmation that
                                             // you are now performing, the indicated option.
#define TELNET_WONT  ((unsigned char) 252)   // Indicates the refusal to perform, or continue performing, the
                                             // indicated option.
#define TELNET_DO    ((unsigned char) 253)   // Indicates the request that the other party perform confirmation
                                             // that you are expecting the other party to perform, the indicated option.
#define TELNET_DONT  ((unsigned char) 254)   // Indicates the demand that the other party stop performing,
                                             // or confirmation that you are no longer expecting the other party
                                             // to perform, the indicated option.
#define TELNET_IAC   ((unsigned char) 255)   // IAC: Data Byte 255.



/*******************************************************************************
 * Telnet option local or remote?
 ******************************************************************************/

enum eOptionMode {
  mInvalid = 0,    // Only for initialization
  mLocal,
  mRemote
};


/*******************************************************************************
 * Telnet option agreement
 ******************************************************************************/

enum eAgreeState {
  sNotSupported = 0,  // Default value
  sNotNegotiated,
  sPending,
  sRequested,
  sAgreed,
  sRejected
};


/*******************************************************************************
 * Telnet state enum
 ******************************************************************************/

enum sTelnetState {
  sTelnetNormal,
  sTelnetInCommand,
  sTelnetWill,
  sTelnetWont,
  sTelnetDo,
  sTelnetDont,
  sTelnetInSubCommand,
  sTelnetEndSubCommand,
  sTelnetReturn
};


#endif
