/*:*
 *: File: ./src/mods/commands/yc_users.cpp
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

#include "../../chat/chat.h"
#include "../../chat/user.h"
/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

extern "C"
{
  int valid_color( string );

  int extern_function(void *v_arg)
  {
    container *c=(container *)v_arg;
    chat* p_chat = (chat*) ((dynamic_wrap*)c->elem[3])->CHAT;
    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;
    user *p_user = (user*)c->elem[1];

    string s_list;
    p_chat->get_user_list(s_list);
    p_user->msg_post( p_conf->get_elem("chat.msgs.userlist") +
                      "<br><table>" + s_list + "</table>\n");

    return 0;
  }
}

