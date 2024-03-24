/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <stdarg.h>
#include <stdio.h>
#include <vdr/tools.h> // Utf8StrLen()
#include "global.h"
#include "telnet.h"
#include "osdstate.h"
#include "term.h"

#define MULTILINE_MARGIN 3U

/*******************************************************************************
 * cTermFont, a dummy font for cTextWrapper
 ******************************************************************************/
class cTermFont : public cFont {
public:
  cTermFont() {}
  #if VDRVERSNUM > 20301
  virtual int Width(void) const { return 1; }
  #endif
  virtual int Width(uint c) const { return 1; } /* int Width(uint c): the only function in use in this class. */
  virtual int Width(const char *s) const { return Utf8StrLen(s); }
  virtual int Height(void) const { return 1; }
  virtual void DrawText(cBitmap *Bitmap, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, int Width) const {}
  virtual void DrawText(cPixmap *Pixmap, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, int Width) const {};
  };
cTermFont termfont;


/*******************************************************************************
 * cOsdState
 ******************************************************************************/
enum eStateMode { sEmptyDisplay, sDisplayMenu, sStatusMsg, sChannelInfo };


cOsdState::cOsdState(cCtrlGateway* parent, const char* name) :
  mOsdMode(sEmptyDisplay), mLastItemPos(-1),
  mParent(parent), mHeight(0), mWidth(0), mScrollPos(0)
{
  mName = name;
  debug_plugin("%s = %p", mName.c_str(), this);
  memset(mTabWidths, 0, sizeof(mTabWidths));
}

cOsdState::~cOsdState() {
  debug_plugin("%s = %p", mName.c_str(), this);
  OsdClear();
}

int cOsdState::MaxLen(const char* Text, int startPos, int endPos) {
  if (Text == NULL)
     return 0;

  const char* text = Text;
  int len = 0;		         // Max. length of string that can be used
  int textPos = startPos;	// Resulting text position
  int tab = -1;
  int tabPos = 0;

  for(;;) {
     char* t = (char*) strchr(text, '\t');
     if (t) { // Tab found - check if the tab can be used
        // Calc next tab pos
        if (++tab < MaxTabs)
           tabPos += mTabWidths[tab] + 1;
        else
           tabPos++;

        if (tabPos <= endPos) {
           len = t - Text + 1;
           textPos = tabPos;
           }
        else { // The tab is out of valid range - shorten the text
           int pendingWidth = t - text;
           if (textPos + pendingWidth > endPos)
              len += endPos - textPos + 1;
           else
              len += pendingWidth;
           // No need to calc the resulting textPos
           break;
           }
        text = t + 1;
        }
     else { // No further tab found
        int pendingWidth = strlen(text);
        if (textPos + pendingWidth > endPos)
           len += endPos - textPos + 1;
        else
           len += pendingWidth;
        // No need to calc the resulting textPos
        break;
        }
     }

  return len;
}

void cOsdState::Send(const char* Msg, ...) {
  char* buf = NULL;
  va_list ap;
  va_start(ap, Msg);
  if (vasprintf(&buf, Msg, ap) > 0) {
     mParent->SendMsg(buf);
     }
  free(buf);
  va_end(ap);
}

void cOsdState::SetSize(int width, int height) {
  if ((width == mWidth) && (height == mHeight))
     return;

  mWidth = width;
  mHeight = height;

  // Clear OSD
  Send(EC_HOME EC_CLRSCR);

  // Recalculate view
  switch(mOsdMode) {
     case sDisplayMenu:
        OsdTitle(mTitle.c_str());
        OsdHelpKeys(mRed.c_str(), mGreen.c_str(), mYellow.c_str(), mBlue.c_str());

        if (mLastItemPos >= 0) {
           debug_plugin("redraw menu");
           mLastItemPos = -1;
           OsdCurrentItem(mCurrentItem.c_str());
           }
        else if (mText.size()) {
           debug_plugin("redraw multi-line-text");
           OsdTextItem(mText.c_str(), false);
           }
        if (mStatusMsg.size())
           OsdStatusMessage(mStatusMsg.c_str());
        break;

     case sStatusMsg:
        OsdStatusMessage(mStatusMsg.c_str());
        break;

     case sChannelInfo:
        OsdChannel(mChannel.c_str());
        OsdProgramme(mPresentTime, mPresentTitle.c_str(),mPresentSubtitle.c_str(),
                     mFollowingTime, mFollowingTitle.c_str(), mFollowingSubtitle.c_str());
        break;

     default:;
     }
}









/*------------------------------------------------------------------------------
 * Indicates a change in the parameters of 'Channel', 'Channel' may
 * require a retune.
 *-----------------------------------------------------------------------------*/
void cOsdState::ChannelChange(const cChannel* Channel) {
  debug_plugin("channel '%s'", Channel&&Channel->Name()?Channel->Name():"NULL");
}

/*------------------------------------------------------------------------------
 * Indicates a change in the timer settings.
 * 'Timer' has been added or will be deleted, respectively.
 *-----------------------------------------------------------------------------*/
void cOsdState::TimerChange(const cTimer* Timer, eTimerChange Change) {
  debug_plugin("timer that has been added or will be deleted '%s'", Timer?*Timer->ToText():"NULL");
}

/*------------------------------------------------------------------------------
 * Indicates a channel switch on the given DVB device.
 * If ChannelNumber is 0, this is before the channel is being switched,
 * otherwise ChannelNumber is the number of the channel that has been switched to.
 * LiveView tells whether this channel switch is for live viewing.
 *-----------------------------------------------------------------------------*/
void cOsdState::ChannelSwitch(const cDevice* Device, int ChannelNumber, bool LiveView) {
  debug_plugin("a channel switch on the DVB device %s: %d", *Device->DeviceName(), ChannelNumber);
}

/*------------------------------------------------------------------------------
 * The given DVB device has started or stopped recording 'Name'.
 *   - 'On': true if started, false otherwise. If On is false, Name may be NULL.
 *   - Name: name of the recording, without directory path.
 *   - FileName: The full file name of recording, may be NULL if no
 *          actual file involved.
 *-----------------------------------------------------------------------------*/
void cOsdState::Recording(const cDevice* Device, const char* Name, const char* FileName, bool On) {
  debug_plugin("Recording on device %s, %s -> %s", *Device->DeviceName(), Name?Name:"NULL", On?"started":"stopped");
}

/*------------------------------------------------------------------------------
 * The given player control has started (On = true) or stopped (On = false) replaying Name.
 *   - Name:  name of recording, without directory path
 *            In case of a player that can't provide a name, Name can be a
 *            string that identifies the player type (like, e.g., "DVD").
 *   - FileName: full file name of recording, may be NULL in case there is no actual
 *            file involved. If On is false, Name may be NULL.
 *-----------------------------------------------------------------------------*/
void cOsdState::Replaying(const cControl* Control, const char* Name, const char* FileName, bool On) {
  debug_plugin("Replaying %s %s", Name, On?"started":"stopped");
}

/*------------------------------------------------------------------------------
 * If the editing marks of the recording that is currently being played are
 * modified in any way, this function is called with the list of Marks.
 * If Marks is NULL, the editing marks for the currently played recording have
 * been deleted entirely.
 *-----------------------------------------------------------------------------*/
void cOsdState::MarksModified(const cMarks* Marks) {
  debug_plugin("Marks modified");
}

/*------------------------------------------------------------------------------
 * The volume has been set to the given value, either abs or rel to current volume.
 *-----------------------------------------------------------------------------*/
void cOsdState::SetVolume(int Volume, bool Absolute) {
  debug_plugin("Set Volume %i, %s", Volume, Absolute?"Absolute":"Relative");
}

/*------------------------------------------------------------------------------
 * The audio track has been set to the one given by Index, which
 * points into the Tracks array of strings. Tracks is NULL-terminated.
 *-----------------------------------------------------------------------------*/
void cOsdState::SetAudioTrack(int Index, const char* const* Tracks) {}

/*------------------------------------------------------------------------------
 * The audio channel has been set to the given value.
 * 0=stereo, 1=left, 2=right, -1=no information available.
 *-----------------------------------------------------------------------------*/
void cOsdState::SetAudioChannel(int AudioChannel) {
  debug_plugin("audio channel has been set: %s",
      AudioChannel==0 ? "stereo" :
      AudioChannel==1 ? "left" :
      AudioChannel==2 ? "right" :
      "no information available");  
}

/*------------------------------------------------------------------------------
 * The subtitle track has been set to the one given by Index, which
 * points into the Tracks array of strings. Tracks is NULL terminated.
 *-----------------------------------------------------------------------------*/
void cOsdState::SetSubtitleTrack(int Index, const char * const *Tracks) {
}

/*------------------------------------------------------------------------------
 * The OSD has been cleared.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdClear() {
  debug_plugin(" ");
  mOsdMode = sEmptyDisplay;
  mScrollPos = 0;
  mLastItemPos = -1;
  mMenu.Clear();
  mTitle.clear();
  mText.clear();
  mRed.clear();
  mGreen.clear();
  mYellow.clear();
  mBlue.clear();
  mStatusMsg.clear();
  mCurrentItem.clear();
  mChannel.clear();
  mPresentTitle.clear();
  mPresentSubtitle.clear();
  mFollowingTitle.clear();
  mFollowingSubtitle.clear();
  memset(mTabWidths, 0, sizeof(mTabWidths));
  mWrapText.Set(NULL, &termfont, mWidth - (MULTILINE_MARGIN << 1));
  Send(EC_HOME EC_CLRSCR);
}

/*------------------------------------------------------------------------------
 * Title has been displayed in the title line of the menu.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdTitle(const char* Title) {
  mOsdMode = sDisplayMenu;

  if (Title and *Title) {
     std::string txt = Title;
     fix_utf8_str(txt);
     const char* title = txt.c_str();

     debug_plugin("%s", title);
     char* t = (char*) strchr(title, '\t');
     if (t) {
        int size = t - title;
        int width = (mWidth - 2) - size;
        if (width < 0) width = 0;
        Send(EC_HOME EC_BG_BL EC_BOLD EC_CLREOL " %-*.*s%*.*s" EC_DEFAULT,
             size, size, title, width, width, t + 1);
        }
     else {
        int pos = (mWidth - 1 - (int)strlen(title)) / 2;
        if (pos < 1)
           pos = 1;
        int width = mWidth - 2 - pos;
        if (width < 0) width = 0;
        debug_plugin("pos %d, width %d, title '%s'", pos, width, title);
        Send(EC_HOME EC_BG_BL EC_BOLD EC_CLREOL "%*s%.*s" EC_DEFAULT,
            pos, "", width, title);
        }
     mTitle = txt;
     }
  else {
     debug_plugin("NULL");
     mTitle.clear();
     Send(EC_HOME EC_BG_BL EC_CLREOL EC_DEFAULT);
     }
}


/*------------------------------------------------------------------------------
 * Message has been displayed in the status line of the menu.
 * If Message is NULL, the status line has been cleared.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdStatusMessage(const char* Message) {
  if (mOsdMode == sEmptyDisplay)
     mOsdMode = sStatusMsg;

  if (Message) {
     std::string msg = Message;
     fix_utf8_str(msg);

     int pos = (mWidth - 1 - msg.size()) / 2;
     if (pos < 1) pos = 1;
     int width = mWidth - 1 - pos;
     if (width < 0) width = 0;
     Send(EC_CURPOS EC_BK_YE EC_CLREOL "%*s%.*s" EC_DEFAULT,
         mHeight - 1, 1, pos, "", width, msg.c_str());
     mStatusMsg = msg;
     }
  else {
     mStatusMsg.clear();
     Send(EC_CURPOS EC_DEFAULT EC_CLREOL, mHeight - 1, 1);
     }
}

/*------------------------------------------------------------------------------
 * The help keys have been set to the given values (may be NULL).
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdHelpKeys(const char* Red, const char* Green, const char* Yellow, const char* Blue) {
  std::string rd, gn, ye, bl;
  int len = max((mWidth / 4) - 2, 0);

  mOsdMode = sDisplayMenu;

  if (Red) {
     rd = Red;
     fix_utf8_str(rd);
     }
  if (Green) {
     gn = Green;
     fix_utf8_str(gn);
     }
  if (Yellow) {
     ye = Yellow;
     fix_utf8_str(ye);
     }
  if (Blue) {
     bl = Blue;
     fix_utf8_str(bl);
     }

  int redWidth    = min((int) rd.size(), len);
  int greenWidth  = min((int) gn.size(), len);
  int yellowWidth = min((int) ye.size(), len);
  int blueWidth   = min((int) bl.size(), len);

  int redSpace    = max((len - redWidth)    / 2, 0);
  int greenSpace  = max((len - greenWidth)  / 2, 0);
  int yellowSpace = max((len - yellowWidth) / 2, 0);
  int blueSpace   = max((len - blueWidth)   / 2, 0);

  Send(EC_CURPOS " %s%*s%.*s%*s" EC_DEFAULT "  %s%*s%.*s%*s" EC_DEFAULT "  %s%*s%.*s%*s" EC_DEFAULT "  %s%*s%.*s%*s" EC_DEFAULT,
    mHeight, 1,
    Red    ? EC_BK_RD : "", redSpace   , "", redWidth   , rd.c_str(), redSpace   , "",
    Green  ? EC_BK_GN : "", greenSpace , "", greenWidth , gn.c_str(), greenSpace , "",
    Yellow ? EC_BK_YE : "", yellowSpace, "", yellowWidth, ye.c_str(), yellowSpace, "",
    Blue   ? EC_WT_BL : "", blueSpace  , "", blueWidth  , bl.c_str(), blueSpace  , "");

  mRed    = rd;
  mGreen  = gn;
  mYellow = ye;
  mBlue   = bl;
}


/*------------------------------------------------------------------------------
 * The OSD displays the given single line Text as menu item at Index.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdItem(const char* Text, int Index) {
  std::string txt;

  if (Text) {
     txt = Text;
     fix_utf8_str(txt);
     }

  debug_plugin("single line at index %d: '%s'", Index, txt.c_str());

  if (Index < mMenu.Count())
     mMenu.Update(Index, Text);
  else
     mMenu.Add(txt.c_str());

  if (txt.size()) {
     const char* found, *lastFound = txt.c_str();
     for(int i = 0; i < MaxTabs && (found = strchr(lastFound, '\t')); ++i) {
        int tabWidth = found - lastFound + 1;
        if (tabWidth > mTabWidths[i])
           mTabWidths[i] = tabWidth;
        lastFound = found + 1;
        }
     }

  mOsdMode = sDisplayMenu;
}


/*------------------------------------------------------------------------------
 * The OSD displays the given single line Text as the current menu item.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdCurrentItem(const char* Text) {
  if (Text == NULL)
     return;

  std::string txt = Text;
  fix_utf8_str(txt);

  debug_plugin("%s (current menu item %d)", txt.c_str(), mLastItemPos);

  int pos = mMenu.Find(txt.c_str());

  // i cannot find this current item. Assume that the item on current pos changed.
  if (pos < 0) {
     if (mLastItemPos >= mMenu.Count())
        return;
     mMenu.Update(mLastItemPos, txt.c_str());
     pos = mLastItemPos;
     }

//  if ((pos < 0) or (pos == mLastItemPos))
//     return;

  const int height = mHeight - 4;
  long oldPage = mLastItemPos < 0 ? -1 : mLastItemPos / height;
  long newPage = pos < 0 ? -1 : pos / height;

  if (oldPage != newPage) {  // Cursor switched to new page?
     // Setting tabs
     Send(EC_CLRTABS); // clear all tabs
     int tabPos = 0;
     for(int i = 0; i < MaxTabs && mTabWidths[i]; ++i) {
        tabPos += mTabWidths[i] + 1;
        Send(EC_CURPOS EC_TABSET, 1, tabPos);
        }
     // Display new page
     long lastEntry = (newPage + 1) * height;
     if (lastEntry > mMenu.Count())
        lastEntry = mMenu.Count();

     for(long i = newPage * height; i < lastEntry; ++i) {
        const char* text = mMenu[i];
        int maxWidth = MaxLen(text, 1, mWidth);
        if (i != pos)
           Send(EC_CURPOS EC_CLREOL "%.*s" EC_DEFAULT,
               (i % height) + 3, 1, maxWidth, text);
        else
           Send(EC_CURPOS EC_BG_BL EC_CLREOL "%.*s" EC_DEFAULT,
               (i % height) + 3, 1, maxWidth, text);
        }

     // Clear the last rows.
     if (lastEntry % height != 0)
        for(long i = lastEntry % height; i < height; ++i)
           Send(EC_CURPOS EC_CLREOL, i + 3, 1);
     }
  else { // Incremental update of current page.
     if (mLastItemPos >= 0) {
        const char* text = mMenu[mLastItemPos];
        int maxWidth = MaxLen(text, 1, mWidth);
        Send(EC_CURPOS EC_CLREOL "%.*s" EC_DEFAULT, // '\033m' here looks like a bug -> 'ESC m' : Memory Unlock (per HP terminals).
            (mLastItemPos % height) + 3, 1, maxWidth, text);
        }
     if (pos >= 0) {
        const char* text = mMenu[pos];
        int maxWidth = MaxLen(text, 1, mWidth);
        Send(EC_CURPOS EC_BG_BL EC_CLREOL "%.*s" EC_DEFAULT,
            (pos % height) + 3, 1, maxWidth, text);
        }
     }
  mCurrentItem = txt.c_str();
  mLastItemPos = pos;
}


/*------------------------------------------------------------------------------
 * The OSD displays the given multi line text. If Text points to an
 * actual string, that text shall be displayed and Scroll has no
 * meaning. If Text is NULL, Scroll defines whether the previously
 * received text shall be scrolled up (true) or down (false) and
 * the text shall be redisplayed with the new offset.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdTextItem(const char* Text, bool Scroll) {
  const int height = mHeight - 4;

  if (Text) {
     std::string txt = Text;
     fix_utf8_str(txt);
     debug_plugin("txt = '%s'", txt.c_str());
     mScrollPos = 0;
     mWrapText.Set(txt.c_str(), &termfont, mWidth - (MULTILINE_MARGIN << 1));
     
     if (txt.c_str() != mText) mText = txt.c_str();
     }
  else {
     debug_plugin("scrolling %s one line.", Scroll? "up":"down");
     if (Scroll) {
        if (mScrollPos > 0)
           mScrollPos--;
        }
     else {
        if (mScrollPos < mWrapText.Lines() - height)
           mScrollPos++;
        }
     }

  int count = min(height, mWrapText.Lines());

  int i;
  for(i = 0; i < count; ++i) {
     const char* text = mWrapText.GetLine(mScrollPos + i);
     debug_plugin("next line %s", text);
     Send(EC_CURPOS EC_CLREOL "%.*s", i + 3, 1, mWidth - 1, text ? text : "");
     }

  if (i < height) {
     // Clear the lowest line in case we scrolled up
     Send(EC_CURPOS EC_CLREOL, i + 3, 1);
     }

  mOsdMode = sDisplayMenu;
}


/*------------------------------------------------------------------------------
 * The OSD displays the single line Text with the current channel information.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdChannel(const char* Text) {
  std::string txt;
  time_t t = time(NULL);
  struct tm ts;
  char buf[32];

  mOsdMode = sChannelInfo;
  strftime(buf, sizeof(buf), "    %a %d %T", localtime_r(&t, &ts));

  if (Text) {
     txt = Text;
     fix_utf8_str(txt);
     }

  mChannel = txt;
  txt += buf;

  Send(EC_CURPOS EC_BK_BL "  " EC_BK_CY " %.*s " EC_BK_BL EC_CLREOL EC_DEFAULT,
       mHeight - 5, 1, mWidth - 10, txt.c_str());
}


/*------------------------------------------------------------------------------
 * The OSD displays the given programme information.
 *-----------------------------------------------------------------------------*/
void cOsdState::OsdProgramme(time_t PresentTime, const char* PresentTitle, const char* PresentSubtitle,
                             time_t FollowingTime, const char* FollowingTitle, const char* FollowingSubtitle) {
  static char buf1[25];
  static char buf2[25];
  struct tm tm_r;
  strftime(buf1, sizeof(buf1), "%R", localtime_r(&PresentTime, &tm_r));
  strftime(buf2, sizeof(buf2), "%R", localtime_r(&FollowingTime, &tm_r));
  std::string title, subtitle, nxtTitle, nxtSubtitle;

  if (PresentTitle) {
     title = PresentTitle;
     fix_utf8_str(title);
     }
  if (PresentSubtitle) {
     subtitle = PresentSubtitle;
     fix_utf8_str(subtitle);
     }
  if (FollowingTitle) {
     nxtTitle = FollowingTitle;
     fix_utf8_str(nxtTitle);
     }
  if (FollowingSubtitle) {
     nxtSubtitle = FollowingSubtitle;
     fix_utf8_str(nxtSubtitle);
     }

  mOsdMode = sChannelInfo;

  Send( EC_CURPOS EC_BK_RD " %s " EC_BG_BL EC_BOLD " %.*s" EC_CLREOL EC_DEFAULT
        EC_CURPOS EC_BK_RD "       " EC_YE_BL "    %.*s" EC_CLREOL
        EC_CURPOS EC_BK_RD "       " EC_BG_BL EC_CLREOL
        EC_CURPOS EC_BK_RD " %s " EC_BG_BL EC_BOLD " %.*s" EC_CLREOL EC_DEFAULT
        EC_CURPOS EC_BK_RD "       " EC_YE_BL "    %.*s" EC_CLREOL EC_DEFAULT,
    mHeight - 4, 1, buf1, mWidth - 10, title.c_str(),
    mHeight - 3, 1,       mWidth - 10, subtitle.c_str(),
    mHeight - 2, 1,
    mHeight - 1, 1, buf2, mWidth - 10, nxtTitle.c_str(),
    mHeight - 0, 1,       mWidth - 10, nxtSubtitle.c_str());

  mPresentTime = PresentTime;
  mFollowingTime = FollowingTime;
  mPresentTitle = title;
  mPresentSubtitle = subtitle;
  mFollowingTitle = nxtTitle;
  mFollowingSubtitle = nxtSubtitle;
}
