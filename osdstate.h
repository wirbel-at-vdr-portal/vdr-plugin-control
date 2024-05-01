/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _CTRLSTATE_H_
#define _CTRLSTATE_H_

#include <string>
#include <vdr/status.h>
#include <vdr/font.h>
#include "global.h"

// forward decls.
class cCtrlGateway;

/*******************************************************************************
 * cOsdState
 ******************************************************************************/

class cOsdState : public cStatus {
public:
  enum { MaxTabs = 6 };
private:
  std::string mName;
  int mOsdMode;
  int mLastItemPos;
  int mTabWidths[MaxTabs];
  std::string mText;
  std::string mTitle;
  std::string mRed;
  std::string mGreen;
  std::string mYellow;
  std::string mBlue;
  std::string mStatusMsg;
  std::string mCurrentItem;
  std::string mChannel;
  std::string mPresentTitle;
  std::string mPresentSubtitle;
  std::string mFollowingTitle;
  std::string mFollowingSubtitle;
  time_t mPresentTime;
  time_t mFollowingTime;
  cTextWrapper mWrapText;
  cStrList mMenu;
  cCtrlGateway* mParent;
  int mHeight;
  int mWidth;
  int mScrollPos;

  int MaxLen(const char* Text, int startPos, int endPos);
  void Send(const char* Msg, ...);
  void Display(int Row);

  virtual void ChannelChange(const cChannel* Channel);
  virtual void TimerChange(const cTimer* Timer, eTimerChange Change);
  virtual void ChannelSwitch(const cDevice* Device, int ChannelNumber, bool LiveView);
  virtual void Recording(const cDevice* Device, const char* Name, const char* FileName, bool On);
  virtual void Replaying(const cControl* Control, const char* Name, const char* FileName, bool On);
  virtual void MarksModified(const cMarks* Marks);
  virtual void SetVolume(int Volume, bool Absolute);
  virtual void SetAudioTrack(int Index, const char* const* Tracks);
  virtual void SetAudioChannel(int AudioChannel);
  virtual void SetSubtitleTrack(int Index, const char * const *Tracks);
  virtual void OsdClear();
  virtual void OsdTitle(const char* Title);
  virtual void OsdStatusMessage(const char* Message);
  virtual void OsdHelpKeys(const char* Red, const char* Green, const char* Yellow, const char* Blue);
  virtual void OsdItem(const char* Text, int Index);
  virtual void OsdCurrentItem(const char* Text);
  virtual void OsdTextItem(const char* Text, bool Scroll);
  virtual void OsdChannel(const char* Text);
  virtual void OsdProgramme(time_t PresentTime, const char* PresentTitle, const char* PresentSubtitle, time_t FollowingTime, const char* FollowingTitle, const char* FollowingSubtitle);

public:
  cOsdState(cCtrlGateway* gateway, const char* name);
  virtual ~cOsdState();
  virtual void SetSize(int width, int height);
};

#endif // _CTRLSTATE_H_
