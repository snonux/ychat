/*:*
 *: File: ./src/main.cpp
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

#include <sys/time.h>
#include <event.h>

#include "incl.h"
#include "sign.h"

#include "maps/hashmap.h"

#ifdef CLI
#include "cli/cli.h"
#endif

using namespace std;

map<string,string>*
parse_argc( int argc, char* argv[] )
{
  map<string,string>* start_params = new map<string,string>;

  string s_output = "";

  // Set to 1 if a config option key has to be read
  // ( ./ychat -o key1 value1 -o key2 value2 ... );
  bool b_conf = 0;

  // Will store the key of an additional option value (see also b_conf)
  string s_key;

  for (int i=1; argv[i] != 0; i++)
  {
    if ( !s_key.empty() )
    {
      (*start_params)[s_key] = string(argv[i]);
      s_key.clear();
    }
    else if ( b_conf )
    {
      s_key = string(argv[i]);
      b_conf = 0;
    }
    else
    {
      if ( string(argv[i]).find("v") != string::npos )
        s_output.append(tool::ychat_version()+"\n");

      if ( string(argv[i]).find("h") != string::npos )
        s_output.append( YCUSAGE );

      if ( string(argv[i]).find("o") != string::npos )
        b_conf = 1;
    }
  }

  if ( !s_output.empty() )
  {
    cout << s_output;
    delete start_params;
    exit(1);
  }

  return start_params;
}

// Single-threaded periodic timer callback (1s) driving the chat timer work.
// Defined here so the event can be registered in main() before event_dispatch.
static void
timer_cb( int /*fd*/, short /*event*/, void* /*arg*/ )
{
  wrap::TIMR->tick();
}

int
main(int argc, char* argv[])
{
  cout << tool::ychat_version() << endl
  << DESCRIP << endl
  << DESCRI2 << endl
  << DESCRI3 << endl
  << CONTACT << endl
  << SEPERAT << endl;

  wrap::init_wrapper(parse_argc(argc, argv));

  //<<*
  // Initialize database connection queue
#ifdef DATABASE

  wrap::DATA->init_connections();
#endif
  //*>>

  event_init();
  sign::init_event_handlers();
  sock::init_event_handlers();

  // Periodic timer in the libevent main loop (single-threaded, no racy
  // pthread): drives time-of-day updates, idle timeouts/auto-away, the
  // stream PING keepalive, ip-cache cleanup and garbage collection. See
  // timr::tick().
  static struct event ev_timer;
  event_set( &ev_timer, -1, EV_PERSIST, timer_cb, NULL );
  struct timeval tv;
  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  event_add( &ev_timer, &tv );

  //wrap::SOCK->start();
  event_dispatch();

  return 0;

  /*
  #ifdef CLI
  // cli _cli; // TODO
  // _cli.start();
  #endif

  // start the socket manager. this one will listen for incoming http requests and will
  // forward them to the specified routines which will generate a http response.
  //wrap::SOCK->start();


  cout << DOWNMSG << endl;
  return 0;
  */
}
