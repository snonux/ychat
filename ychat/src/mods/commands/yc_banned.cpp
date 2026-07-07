/*:*
 *: File: ./src/mods/commands/yc_banned.cpp
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

#include "../../chat/user.h"
#include "../../chat/chat.h"
#include "../../maps/shashmap.h"

using namespace std;

extern "C"
{
  int extern_function(void *v_arg)
  {
    container *c=(container *)v_arg;

    user *p_user = (user*) c->elem[1];
    chat* p_chat = (chat*) ((dynamic_wrap*)c->elem[3])->CHAT;
    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;

    shashmap<string>* map_banned_nicks = p_chat->get_map_banned_nicks();
    string s_msg("");

    if ( map_banned_nicks->size() > 0 )
    {
      vector<string>* vec_keys = map_banned_nicks->get_key_vector();

      for (vector<string>::iterator iter = vec_keys->
                                           begin();
           iter != vec_keys->end();
           ++iter)
        s_msg.append(*iter + ": " + map_banned_nicks->get_elem(*iter) + "<br>\n");

    }
    else
    {
      s_msg = "<font color=\"#"
              + p_conf->get_elem("chat.html.errorcolor")
              + "\">"
              + p_conf->get_elem("chat.msgs.err.nobanned")
              + "</font><br>\n";
    }

    p_user->msg_post( &s_msg );
    return 0;
  }
}

