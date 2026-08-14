/*:*
 *: File: ./src/monitor/dump.h
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.5-CURRENT
 *: 
 *: Copyright (C) 2003 Paul C. Buetow, Volker Richter
 *: Copyright (C) 2004 Paul C. Buetow
 *: Copyright (C) 2005 EXA Digital Solutions GbR
 *: Copyright (C) 2006, 2007 Paul C. Buetow
 *: 
 *: This program is free software; you can redistribute it and/or
 *: modify it under the terms of the GNU General Public License
 *: as published by the Free Software Foundation; either version 2
 *: of the License, or (at your option) any later version.
 *: 
 *: This program is distributed in the hope that it will be useful,
 *: but WITHOUT ANY WARRANTY; without even the implied warranty of
 *: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *: GNU General Public License for more details.
 *: 
 *: You should have received a copy of the GNU General Public License
 *: along with this program; if not, write to the Free Software
 *: Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *:*/

#include "../incl.h"

#ifndef DUMP_H
#define DUMP_H

using namespace std;

class dumpable
{
private:
  int i_level;
  int i_lined;
  bool b_lined;
  bool b_next_no_nl;
  string s_dump;

  static const string s_sep;
  static const int i_max_level;

  virtual void dumpit() = 0;
  void initialize(int i_level);
  void reset();

protected:
  void add
  (unsigned i_num)
  {
    add
    ("<unsigned>");
  }

  void add
  (int i_num)
  {
    add
    ("<int>");
  }

  void add
  (string s_line);
  dumpable();

  void next_no_newline()
  {
    b_next_no_nl = true;
  }

public:
  string dump();
  string dump(int i_level);
  int get_level() const;
};

class dump
{
private:
  string run(vector<string> &vec_params);
public:
  dump(vector<string> vec_params);
};

#endif
