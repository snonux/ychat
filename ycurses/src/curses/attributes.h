#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <set>
#include "incl.h"
#include "color.h"

using namespace std;

const int Normal = A_NORMAL;
const int Standout = A_STANDOUT;
const int Underline = A_UNDERLINE; 
const int Reverse = A_REVERSE; 
const int Blink = A_BLINK;
const int Dim = A_DIM;
const int Bold = A_BOLD;
const int Protect = A_PROTECT;
const int Invis = A_INVIS;
const int AltCharSet = A_ALTCHARSET;
const int CharText = A_CHARTEXT;

class attributes 
{
private:
  void init();
  set<int> set_attr;
  bool get(int i_attr);
  // Named set_attr_flag (not "set") because a member named exactly "set"
  // here would collide with std::set - "using namespace std" makes both
  // visible and GCC treats declaring the member as ill-formed ("changes
  // meaning of 'set'"). Same class of fix as the function->mod_func_t
  // rename in ychat/yhttpd's glob.h.
  void set_attr_flag(bool b, int i_attr);
  color* p_color;

  friend class window;
  void use_wattron(WINDOW* p_window);
  void use_wattroff(WINDOW* p_window);

public:
  attributes();
  attributes(int i_attr);
  attributes(color& r_color);

  void set_attr_flag(int i_attr) { set_attr_flag(true, i_attr); }
  void unset(int i_attr ) { set_attr_flag(false, i_attr); }
  void unset_all();
  void set_color(color& r_color);

  void set_normal(bool b) { set_attr_flag(b, Normal); }
  void set_standout(bool b) { set_attr_flag(b, Standout); }
  void set_underline(bool b) { set_attr_flag(b, Underline); }
  void set_reverse(bool b) { set_attr_flag(b, Reverse); }
  void set_blink(bool b) { set_attr_flag(b, Blink); }
  void set_dim(bool b) { set_attr_flag(b, Dim); }
  void set_bold(bool b) { set_attr_flag(b, Bold); }
  void set_protect(bool b) { set_attr_flag(b, Protect); }
  void set_invisible(bool b) { set_attr_flag(b, Invis); }
  void set_altcharset(bool b) { set_attr_flag(b, AltCharSet); }
  void set_chartext(bool b) { set_attr_flag(b, CharText); }

  bool get_normal() { return get(Normal); }
  bool get_standout() { return get(Standout); }
  bool get_underline() { return get(Underline); }
  bool get_reverse() { return get(Reverse); }
  bool get_blink() { return get(Blink); }
  bool get_dim() { return get(Dim); }
  bool get_bold() { return get(Bold); }
  bool get_protect() { return get(Protect); }
  bool get_invisible() { return get(Invis); }
  bool get_altcharset() { return get(AltCharSet); }
  bool get_chartext() { return get(CharText); }
};

#endif
