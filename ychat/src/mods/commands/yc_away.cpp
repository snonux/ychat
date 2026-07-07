/*:*
 *: File: ./src/mods/commands/yc_away.cpp
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
#include "../../chat/room.h"
#include "../../chat/user.h"

using namespace std;

extern "C"
{
  int extern_function(void *v_arg)
  {
    container *c=(container *)v_arg;

    user* p_user = (user*) c->elem[1];		// the corresponding user
    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;
    timr* p_timr = (timr*) ((dynamic_wrap*)c->elem[3])->TIMR;
    vector<string> *params = (vector<string>*) c->elem[2];	// param array

    string s_msg;
    string s_away;
    string s_col;

    vector<string>::iterator iter;

    string s_time = p_timr->get_time();

    s_away.append( s_time ).append( " " );

    s_msg = s_time
            + " "
            + p_user->get_colored_bold_name()
            + " "
            + p_conf->get_elem("chat.msgs.setmodeaway")
            + "<font color=" + p_user->get_col2() + ">";

    if ( params->size() > 0 )
    {
      s_msg.append(": ");
      for ( iter = params->begin(); iter != params->end(); iter++ )
      {
        s_msg.append( " " + *iter );
        s_away.append( *iter + " " );
      }
    }
    else
    {
      s_msg.append(".");
    }

    s_msg.append( "</font><br>\n" );

    room* p_room = p_user->get_room();

    // Remove ' from away message:
    unsigned long pos;

    do
    {
      pos = s_away.find_first_of( "'" );
      if ( pos == string::npos )
        break;
      s_away.replace( pos, 1, "\"" );
    }
    while (true);

    p_user->set_away( true, s_away );
    p_room->reload_onlineframe();
    p_room->msg_post ( &s_msg  );

    return 0;
  }
}

