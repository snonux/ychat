/*:*
 *: File: ./src/mods/commands/yc_morph.cpp
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
#include "../../chat/room.h"
#include "../../tool/tool.h"

/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

extern "C"
{
  int extern_function(void *v_arg)
  {
    container *c = (container*) v_arg;

    user *p_user = (user*) c->elem[1];		// the corresponding user
    vector<string> *p_params = (vector<string>*) c->elem[2];	// param array
    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;
    timr* p_timr = (timr*) ((dynamic_wrap*)c->elem[3])->TIMR;

    if ( p_params->empty() )
    {
      string s_msg = p_conf->colored_error_msg("chat.msgs.err.wrongcommandusage");
      p_user->msg_post(s_msg);
    }
    else
    {
      string s_morphed_name = p_params->at(0);

      if ( tool::to_lower(s_morphed_name) != p_user->get_lowercase_name() )
      {
        string s_msg = p_conf->colored_error_msg("chat.msgs.err.morphnick");
        p_user->msg_post(s_msg);
      }
      else
      {
        string s_msg = p_timr->get_time() + " " + p_user->get_colored_bold_name() + p_conf->get_elem( "chat.msgs.usermorphs" ) + "<font color=\"#" + p_user->get_col1() + "\"><b>" + s_morphed_name + "</b></font><br>\n";

        room* p_room = p_user->get_room();
        p_room->msg_post( &s_msg );
        p_user->set_name( s_morphed_name );
        p_room->reload_onlineframe();
      }
    }

    return 0;
  }

}


