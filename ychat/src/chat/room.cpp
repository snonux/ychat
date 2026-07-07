/*:*
 *: File: ./src/chat/room.cpp
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

#ifndef ROOM_CPP
#define ROOM_CPP

#include "room.h"
#include "../maps/mtools.h"

using namespace std;

room::room( string s_name ) : name( s_name )
{
#ifdef LOGGING

  p_logd = new logd( wrap::CONF->get_elem("chat.logging.roomlogdir") + get_lowercase_name(),
                     wrap::CONF->get_elem("chat.logging.roomloglines") );
#endif

  wrap::STAT->increment_num_rooms();
}

room::~room()
{
  wrap::STAT->decrement_num_rooms();

  // Delete all user objects
  run_func( mtools<user*>::delete_obj );

#ifdef LOGGING

  delete p_logd;
#endif

}

string
room::get_topic()
{
  string s_ret;
  s_ret = s_topic;
  return s_ret;
}

void
room::set_topic( string s_topic )
{
  if ( s_topic == "" )
    this->s_topic = "";
  else
    this->s_topic = s_topic + "<br><br>";
  reload_onlineframe();
}

void
room::set_topic( string s_topic, string s_color )
{
  set_topic( "<font color=\"#" + s_color + "\">" + s_topic + "</font>");
}

void
room::clean_room()
{
  this->s_topic = "";
  wrap::CHAT->del_elem( get_lowercase_name() );
  wrap::GCOL->add_room_to_garbage( this );
}

void
room::reload_onlineframe()
{
  javascript_post("parent.online.location.reload();");
}

void
room::set_name( string s_name )
{
  if ( tool::to_lower(s_name) == get_lowercase_name() )
  {
    name::set_name( s_name );
    return;
  }

#ifdef LOGGING
  p_logd->flush_logs();
#endif

  if ( s_name == "" )
    return;

  name::set_name( s_name );
#ifdef LOGGING

  p_logd->set_logfile( wrap::CONF->get_elem("chat.logging.roomlogdir"), get_lowercase_name() );
#endif
}

void
room::dumpit()
{
  dumpable::add
  ("[room]");
  dumpable::add
  ("Name: "+get_name());
  dumpable::add
  ("Topic: "+get_topic());
  base<user>::dumpit();
}


#endif
