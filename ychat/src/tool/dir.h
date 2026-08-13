/*:*
 *: File: ./src/tool/dir.h
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.4-CURRENT
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


#ifndef DIR_H
#define DIR_H

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include <vector>

#include "../incl.h"

using namespace std;

class dir
{
private:
  bool b_open;
  DIR *p_d;
  struct dirent *p_ep;
  vector<string> vec_dir;

public:
  dir();
  ~dir();

  bool open_dir( char *c_dir );
  bool open_dir( string &s_dir );
  void close_dir();
  void read_dir();
  vector<string> get_dir_vec();
};

#endif
