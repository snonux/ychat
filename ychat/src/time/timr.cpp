/*:*
 *: File: ./src/time/timr.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.3-CURRENT
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

#ifndef TIMR_CPP
#define TIMR_CPP

#include <sys/time.h>
#include "timr.h"

using namespace std;

timr::timr()
{
  wrap::system_message( TIMERIN );
  b_timer_active = true;


  i_time_offset = tool::string2int( wrap::CONF->get_elem("chat.timeoffset") );
  wrap::system_message( TIMEROF + tool::int2string( i_time_offset ) );

  s_time = "00:00:00";
  s_uptime = "00:00:00";
}

timr::~timr()
{}

bool
timr::get_timer_active() const
{
  return b_timer_active;
}

int
timr::get_offset()
{
  int i_ret_val = i_time_offset;
  return i_ret_val;
}

void
timr::start( void *v_ptr )
{
  wrap::system_message( TIMERTH );

  time_t clock_start;
  time_t clock_now;

  time( &clock_start );
  tm time_start = *localtime( &clock_start );
  tm time_now;

  while ( get_timer_active() )
  {
    // sleep a second!
    usleep( 1000000 );

    // get the current time!
    time( &clock_now );

    time_now = *localtime( &clock_now );

    // set the current time && the current ychat uptime!
    set_time( difftime( clock_now, clock_start ),
              time_now.tm_sec, time_now.tm_min, time_now.tm_hour );

    // run every minute:
    if ( time_now.tm_sec == 0 )
    {
#ifdef SERVMSG
      cout << TIMERUP << get_uptime() << endl;
#endif
      //<<*
      int* p_timeout_settings = new int[3];
      p_timeout_settings[0] = tool::string2int(wrap::CONF->get_elem("chat.idle.timeout"));
      p_timeout_settings[1] = tool::string2int(wrap::CONF->get_elem("chat.idle.awaytimeout"));
      p_timeout_settings[2] = tool::string2int(wrap::CONF->get_elem("chat.idle.autoawaytimeout"));
      wrap::CHAT->check_timeout( p_timeout_settings );
      delete p_timeout_settings;

      string s_ping = "<!-- PING! //-->\n";
      wrap::CHAT->msg_post( &s_ping );

#ifdef DATABASE
      // Disconnecting idle database conenction
      wrap::DATA->check_data_con_timeout();
#endif
      //*>>
      // Run every ten minutes:
      if ( time_now.tm_min % 10 == 0 )
      {

        wrap::SOCK->clean_ipcache();
        // Run every hour
        if ( time_now.tm_min % 60 == 0 )
        {
          wrap::GCOL->remove_garbage(); //<<

          // Run every day
          if (time_now.tm_min == 0 || time_now.tm_min == 60 )
            if (time_now.tm_hour == 0 || time_now.tm_hour == 24)
              wrap::STAT->update_rusage_history();
        }
      }
    }
  }
}

// Single-threaded periodic tick, called from the libevent main loop (see
// main.cpp) instead of the old, never-finished racy timer thread
// (TIMR->run() was referenced but undefined; timr::start() ran in a pthread
// and raced with the event loop). One tick == one second. Per-minute work
// runs when the wall-clock second is 0, matching the old cadence.
void
timr::tick()
{
  static time_t clock_start = 0;
  if ( clock_start == 0 )
    time( &clock_start );

  time_t clock_now;
  time( &clock_now );
  tm time_now = *localtime( &clock_now );

  set_time( difftime( clock_now, clock_start ),
            time_now.tm_sec, time_now.tm_min, time_now.tm_hour );

  if ( time_now.tm_sec != 0 )
    return; // per-minute work only at the top of each minute

  // Idle-timeout / auto-away sweep. Safe now that hashmap::run_func
  // snapshots the values before iterating (set_online(false) deletes the
  // user from its room mid-iteration).
  int* p_timeout_settings = new int[3];
  p_timeout_settings[0] = tool::string2int(wrap::CONF->get_elem("chat.idle.timeout"));
  p_timeout_settings[1] = tool::string2int(wrap::CONF->get_elem("chat.idle.awaytimeout"));
  p_timeout_settings[2] = tool::string2int(wrap::CONF->get_elem("chat.idle.autoawaytimeout"));
  wrap::CHAT->check_timeout( p_timeout_settings );
  delete p_timeout_settings;

  // Keep stream frames alive and detect dead connections.
  string s_ping = "<!-- PING! //-->\n";
  wrap::CHAT->msg_post( &s_ping );

  if ( time_now.tm_min % 10 == 0 )
    wrap::SOCK->clean_ipcache();

  if ( time_now.tm_min == 0 )
    wrap::GCOL->remove_garbage();
}

void
timr::set_time( double d_uptime, int i_cur_seconds, int i_cur_minutes, int i_cur_hours )
{

  int i_hours = (int) d_uptime / 3600;
  int i_minutes = (int) d_uptime / 60;

  while ( i_minutes >= 60 )
    i_minutes -= 60;

  while ( d_uptime >= 60 )
    d_uptime -= 60;

  // Calculate offset time
  i_cur_hours += get_offset();

  for ( int i = 24-i_cur_hours; i < 0; i = 24-i_cur_hours )
    i_cur_hours =- i;

  if (i_cur_hours == 24)
    i_cur_hours = 0;

  s_time = add_zero_to_front( tool::int2string( i_cur_hours ) ) + ":" +
           add_zero_to_front( tool::int2string( i_cur_minutes ) ) + ":" +
           add_zero_to_front( tool::int2string( i_cur_seconds ) );

  s_uptime = add_zero_to_front( tool::int2string( i_hours ) ) + ":" +
             add_zero_to_front( tool::int2string( i_minutes ) )  + ":" +
             add_zero_to_front( tool::int2string( (int) d_uptime ) );
}

string
timr::add_zero_to_front( string s_time )
{
  if ( s_time.length() == 1 )
  {
    string s_new = "0" + s_time;
    return s_new;
  }

  return s_time;
}

double
timr::get_time_diff( time_t &clock_diff )
{
  time_t clock_now;
  time( &clock_now );

  return difftime(clock_now, clock_diff);
}
#endif
