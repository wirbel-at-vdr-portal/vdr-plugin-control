/* control - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include "global.h"
#include "keyboard.h"

struct tKeymap {
  eKbdFunc func;
  uint64_t code;
};

static const tKeymap keymap[] = {
  { kfF1,     0x0000001B5B31317EULL },
  { kfF1,     0x000000001B5B5B41ULL },
  { kfF1,     0x00000000009B5B41ULL },
  { kfF1,     0x0000000000001B31ULL },
  { kfF2,     0x0000001B5B31327EULL },
  { kfF2,     0x000000001B5B5B42ULL },
  { kfF2,     0x00000000009B5B42ULL },
  { kfF2,     0x0000000000001B32ULL },
  { kfF3,     0x0000001B5B31337EULL },
  { kfF3,     0x000000001B5B5B43ULL },
  { kfF3,     0x00000000009B5B43ULL },
  { kfF3,     0x0000000000001B33ULL },
  { kfF4,     0x0000001B5B31347EULL },
  { kfF4,     0x000000001B5B5B44ULL },
  { kfF4,     0x00000000009B5B44ULL },
  { kfF4,     0x0000000000001B34ULL },
  { kfF5,     0x0000001B5B31357EULL },
  { kfF5,     0x000000001B5B5B45ULL },
  { kfF5,     0x00000000009B5B45ULL },
  { kfF5,     0x0000000000001B35ULL },
  { kfF6,     0x0000001B5B31377EULL },
  { kfF6,     0x000000001B5B5B47ULL },
  { kfF6,     0x00000000009B5B47ULL },
  { kfF6,     0x0000000000001B36ULL },
  { kfF7,     0x0000001B5B31387EULL },
  { kfF7,     0x000000001B5B5B48ULL },
  { kfF7,     0x00000000009B5B48ULL },
  { kfF7,     0x0000000000001B37ULL },
  { kfF8,     0x0000001B5B31397EULL },
  { kfF8,     0x000000001B5B5B49ULL },
  { kfF8,     0x00000000009B5B49ULL },
  { kfF8,     0x0000000000001B38ULL },
  { kfF9,     0x0000001B5B32307EULL },
  { kfF9,     0x000000001B5B5B3AULL },
  { kfF9,     0x00000000009B5B3AULL },
  { kfF9,     0x0000000000001B39ULL },
  { kfF10,    0x0000001B5B32317EULL },
  { kfF10,    0x000000001B5B5B3BULL },
  { kfF10,    0x00000000009B5B3BULL },
  { kfF10,    0x0000000000001B3AULL },
  { kfF11,    0x0000001B5B32327EULL },
  { kfF11,    0x000000001B5B5B3DULL },
  { kfF11,    0x00000000009B5B3DULL },
  { kfF12,    0x0000001B5B32337EULL },
  { kfF12,    0x000000001B5B5B3EULL },
  { kfF12,    0x00000000009B5B3EULL },
  { kfUp,     0x00000000001B5B41ULL },
  { kfDown,   0x00000000001B5B42ULL },
  { kfLeft,   0x00000000001B5B44ULL },
  { kfRight,  0x00000000001B5B43ULL },
  { kfHome,   0x00000000001B5B48ULL },
  { kfEnd,    0x00000000001B5B46ULL },
  { kfPgUp,   0x000000001B5B357EULL },
  { kfPgDown, 0x000000001B5B367EULL },
  { kfIns,    0x000000001B5B327EULL },
  { kfDel,    0x000000001B5B337EULL },
  { kfNone,   0x0000000000000000ULL },
};

/*******************************************************************************
 * cCtrlKeyboard
 ******************************************************************************/

cCtrlKeyboard::cCtrlKeyboard(const char* name) : cKbdRemote() {
  mName = name;
  debug_plugin("'%s' = %p", mName.c_str(), this);
}

cCtrlKeyboard::~cCtrlKeyboard() {
  debug_plugin("'%s' = %p", mName.c_str(), this);
}

bool cCtrlKeyboard::Put(uint64_t Command/*, bool Repeat, bool Release*/) {
  if      (Command == 0x1B5B31317EULL) return cRemote::Put(kRed);
  else if (Command == 0x1B5B31327EULL) return cRemote::Put(kGreen);
  else if (Command == 0x1B5B31337EULL) return cRemote::Put(kYellow);
  else if (Command == 0x1B5B31347EULL) return cRemote::Put(kBlue);

  return cRemote::Put(Command, false, false);
}

uint64_t cCtrlKeyboard::MapFuncToCode(int Func) {
  for(const tKeymap* p = keymap; p->func != kfNone; p++) {
     if (p->func == Func)
        return p->code;
     }
  return (Func <= 0xFF) ? Func : 0;
}

int cCtrlKeyboard::MapCodeToFunc(uint64_t Code) {
  for(const tKeymap* p = keymap; p->func != kfNone; p++)
     if (p->code == Code)
        return p->func;

  if (Code <= 0xFF)
     return Code;

  return kfNone;
}
