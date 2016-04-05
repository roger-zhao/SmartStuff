/*
 * smb_menu.cpp
 *
 */

#include "smb_menu.h"



Menu::InputType Menu::input;
Menu::OutputType Menu::output;

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Default input function is _input(). Default display method is _default()


Menu::Menu(const char *_n, unsigned char _id, OutputType _outType, InputType _inType, displayFlg _flg) {
		this->name = _n; this->id = _id; this->output = _outType; this->input = _inType;
		this->disFlg = _flg; this->tabId = -1;
};

Menu::~Menu() {;}



