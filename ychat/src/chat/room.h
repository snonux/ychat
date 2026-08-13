/*:*
 *: File: ./src/chat/room.h
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

// class room declaration.

#include "../incl.h"
#include "../wrap.h"

#ifndef ROOM_H
#define ROOM_H

#include "base.h"
#include "../name.h"
#include "user.h"
#include "../tool/tool.h"
#include "../logd.h"

using namespace std;

class room : public base<user>, public name
{
private:
  string s_topic;
#ifdef LOGGING

  logd* p_logd;
#endif

  void dumpit();

public:
  room( string s_name );
  ~room();

  virtual void reload_onlineframe();

  void add_user( user* p_user  )
  {
    add_elem( p_user );
    p_user->set_p_room( this );
    reload_onlineframe();
  }

  user* get_user( string &s_name, bool &b_found )
  {
    return static_cast<user*>( get_elem( s_name, b_found ) );
  }

  void del_elem( string &s_name )
  {
    base<user>::del_elem( s_name );

    if ( base<user>::size() == 0 )
      clean_room();

    else
      reload_onlineframe();
  }

  void msg_post( string s_msg )
  {
    msg_post( &s_msg );
  }

  void msg_post( string *p_msg )
  {
#ifdef LOGGING
    p_logd->log_simple_line( logd::remove_html_tags(*p_msg) );
#endif
    base<user>::msg_post( p_msg );
  }

  void javascript_post( string s_msg )
  {
    string s_js = "<script language=\"JavaScript\">"+s_msg+"</script>\n";
    base<user>::msg_post( &s_js );
  }

  string get_bold_name()
  {
    return "<b>" + get_name() + "</b>";
  }

  virtual void set_name( string s_name );
  string get_topic();
  void set_topic( string s_topic );
  void set_topic( string s_topic, string s_color );
  void clean_room();



};

#endif
