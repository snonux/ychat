/*:*
 *: File: ./src/mods/commands/yc_getroom.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.1-CURRENT
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

#include "../../incl.h"

/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

extern "C"
{
  int extern_function(void *v_arg)
  {

    container *c=(container *)v_arg;

    user *p_user = (user*) c->elem[1];		// the corresponding user
    vector<string> *params = (vector<string>*) c->elem[2];	// param array
    chat* p_chat = (chat*) ((dynamic_wrap*)c->elem[3])->CHAT;
    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;

    if ( ! params->empty() )
    {
      vector<string>::iterator iter = params->begin();
      string s_searched_user( *iter );
      bool b_found;
      user* p_searched_user = p_chat->get_user( s_searched_user, b_found );

      if ( b_found )
      {
        string s_msg         = "<b>" + p_searched_user->get_colored_name() + "</b>"
                               + p_conf->get_elem("chat.msgs.userinroom") + "<b>"
                               + p_searched_user->get_room()->get_name()
                               + "</b><br>\n";
        p_user->msg_post( &s_msg );
      }
      else
      {
        string s_msg = "<font color=\"#"
                       + p_conf->get_elem("chat.html.errorcolor")
                       + "\"><b>"
                       + s_searched_user + "</b> "
                       + p_conf->get_elem("chat.msgs.err.notavailable")
                       + "</font><br>\n";
        p_user->msg_post( &s_msg );

      }
    }

    return 0;
  }

}

