#ifndef ATTRIBUTES_CPP
#define ATTRIBUTES_CPP

#include "attributes.h"

attributes::attributes()
{
  init();
}

attributes::attributes(int i_attr)
{
  init();
  set_attr_flag(true, i_attr);
}

attributes::attributes(color& r_color)
{
  init();
  set_color(r_color);
}

void
attributes::init()
{
  p_color = 0;
}

void
attributes::use_wattron(WINDOW* p_window)
{
  std::set<int>::iterator iter;
  for (iter = set_attr.begin(); iter != set_attr.end(); ++iter)
    wattron(p_window, *iter);

  if (p_color)
    wattron(p_window, COLOR_PAIR(p_color->get_num()));
}

void
attributes::use_wattroff(WINDOW* p_window)
{
  std::set<int>::iterator iter;
  for (iter = set_attr.begin(); iter != set_attr.end(); ++iter)
    wattroff(p_window, *iter);

  if (p_color)
    wattroff(p_window, COLOR_PAIR(p_color->get_num()));
}


void
attributes::unset_all()
{
  set_attr.clear();
  p_color = 0;
}

bool
attributes::get(int i_attr)
{
  return set_attr.find(i_attr) != set_attr.end();   
}

void
attributes::set_attr_flag(bool b, int i_attr)
{
  if ((set_attr.find(i_attr) != set_attr.end() ) == b)
    return;

  set_attr.insert(i_attr);
}

void
attributes::set_color(color& r_color)
{
  this->p_color = &r_color;
}

#endif
