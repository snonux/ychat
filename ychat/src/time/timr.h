/*:*
 *: File: ./src/time/timr.h
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.2-CURRENT
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

#ifndef TIMR_H
#define TIMR_H

#include <unistd.h>

using namespace std;

class timr
{
private:
  bool b_timer_active;
  int  i_time_offset;
  string s_uptime;
  string s_time;


public:
  timr();
  ~timr();

  bool get_timer_active() const;
  void start( void *v_ptr );
  void tick(); // single-threaded periodic tick (called from the libevent loop)

  void set_time( double d_uptime, int i_cur_seconds, int i_cur_minutes, int i_cur_hours );
  string add_zero_to_front( string s_time );

  // inline for dynamic module access!
  string
  get_time(  )
  {
    string s_ret;
    s_ret = this->s_time;
    return s_ret;
  }

  string
  get_uptime(  )
  {
    string s_ret;
    s_ret = this->s_uptime;
    return s_ret;
  }

  int get_offset();
  double get_time_diff( time_t &clock_diff );
};

#endif
