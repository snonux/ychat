/*:*
 *: File: ./src/logd.h
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

#include "incl.h"

#ifdef LOGGING
#ifndef LOGD_H
#define LOGD_H

#include <queue>
#include <time.h>

class logd
{
private:
  string s_logfile;
  queue<string> s_queue;
  int i_lines;

  void initialize( string s_filename, int i_log_lines );
  void flush();
  void set_lines( const int i_lines );
  string get_time_string();

public:
  logd( string s_filename, string s_log_lines );
  logd( string s_filename, int i_log_lines );
  ~logd();

  void set_logfile( string s_path, string s_filename );
  void log_access( map<string,string> &map_request );
  void log_simple_line( string s_line );
  void flush_logs();
  static string remove_html_tags( string s_log );
};

#endif
#endif

