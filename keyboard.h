/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _CTRLKEYBOARD_H_
#define _CTRLKEYBOARD_H_

#include <vdr/remote.h>
#include <vdr/thread.h>

/*******************************************************************************
 * cCtrlKeyboard
 ******************************************************************************/
class cCtrlKeyboard : public cKbdRemote  {
private:
  std::string mName;
public:
  cCtrlKeyboard(const char* name);
  virtual ~cCtrlKeyboard();
  bool Put(uint64_t Command);
  static int MapCodeToFunc(uint64_t Code);
  uint64_t MapFuncToCode(int Func);
};

#endif
