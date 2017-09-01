#include <assert.h>
#include "../init.h"
#include "fltkkbd.h"
#include <FL/Fl_Repeat_Button.H>

constexpr int KeyboardRows = 9;
constexpr int KeyboardColumns = 6;

static const char* kCharForKey[Ion::Keyboard::NumberOfKeys] = {
  "Left",   "Up",     "Down",   "Right",  "OK",     "Back",
  "Home",   "Power",  "",       "",       "",       "",
  "Shift",  "Alpha",  "x,n,t",  "Var",    "Toolbox",  "Clear",
  "e^x",    "ln",     "log",    "i",      ",",      "x^y",
  "sin",    "cos",    "tan",    "pi",     "root",   "square",
  "7",      "8",      "9",      "(",      ")",      "",
  "4",      "5",      "6",      "x",      "%",      "",
  "1",      "2",      "3",      "+",      "-",      "",
  "0",      ".",      "x10^x",  "Ans",    "EXE",    ""
};

static const int kShortcutForKey[Ion::Keyboard::NumberOfKeys] = {
  FL_Left, FL_Up, FL_Down, FL_Right, FL_Enter, FL_Escape,
  FL_Home, 0, 0, 0, 0, 0,
  0, 0, 'x', 0, 0, FL_BackSpace,
  0, 0, 0, 'i', ',', 0,
  0, 0, 0, 0, 0, 0,
  '7', '8', '9', '(',')', 0,
  '4', '5', '6', '*', '/', 0,
  '1', '2', '3', '+', '-', 0,
  '0', '.', 0, 0, FL_KP_Enter, 0
};

static bool shouldRepeatKey(Ion::Keyboard::Key k) {
  return k <= Ion::Keyboard::Key::A4 || k == Ion::Keyboard::Key::A6;
}

#include <iostream>
static void keyHandler(Fl_Widget *, long key) {
  if (currentEvent == Ion::Events::None) {
    currentEvent = Ion::Events::Event(Ion::Keyboard::Key(key),
                                      Ion::Events::isShiftActive(),
                                      Ion::Events::isAlphaActive());
  }
}

FltkKbd::FltkKbd(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
  assert(KeyboardRows*KeyboardColumns == Ion::Keyboard::NumberOfKeys);
  int key_width = w/KeyboardColumns;
  int key_height = h/KeyboardRows;
  for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
    Fl_Button * button;
    int key_x = x + (k%KeyboardColumns)*key_width;
    int key_y = y + (k/KeyboardColumns)*key_height;
    if (shouldRepeatKey((Ion::Keyboard::Key)k)) {
      button = new Fl_Repeat_Button(key_x, key_y, key_width, key_height, kCharForKey[k]);
    } else {
      button = new Fl_Button(key_x, key_y, key_width, key_height, kCharForKey[k]);
    }
    if (kCharForKey[k][0] == '\0') {
      button->deactivate();
    } else {
      button->callback(keyHandler, k);
    }
    if (kShortcutForKey[k]) {
      button->shortcut(kShortcutForKey[k]);
    }
    button->clear_visible_focus();
  }
  end();
}

bool FltkKbd::key_down(Ion::Keyboard::Key key) {
  return m_buttons[(int)key]->value();
}
