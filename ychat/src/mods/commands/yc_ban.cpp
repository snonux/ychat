/*:*
 *: File: ./src/mods/commands/yc_ban.cpp
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

#include "../../chat/user.h"
#include "../../chat/room.h"

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
    timr* p_timr = (timr*) ((dynamic_wrap*)c->elem[3])->TIMR;

    if ( ! params->empty() )
    {
      vector<string>::iterator iter = params->begin();
      string s_bannick_user(*iter);
      //if (string.find(".") != string::npos) {
      //}

      bool b_found;
      user* p_bannick_user = p_chat->get_user( s_bannick_user, b_found );

      if ( b_found )
      {
        string s_reason = "";
        for ( iter++; iter != params->end(); iter++ )
          s_reason.append( *iter + " " );

        bool b_reason = s_reason.length() > 0;

        string s_retmsg = p_chat->ban_nick(s_bannick_user, string("( " + p_user->get_colored_name() + (b_reason ? ": " : "") + s_reason + " )"));
        if (!s_retmsg.empty())
        {
          string s_msg = "<font color=\"#"
                         + p_conf->get_elem("chat.html.errorcolor")
                         + "\"><b>"
                         + s_bannick_user + "</b> "
                         + p_conf->get_elem("chat.msgs.err.alreadybanned")
                         + " " + s_retmsg
                         + "</font><br>\n";

          p_user->msg_post( &s_msg );
          return 0;
        }

        string s_time = "";
        if ( p_conf->get_elem("chat.printalwaystime") == "true" )
          s_time = p_timr->get_time() + " ";

        string s_msg = s_time + "<i> " + p_user->get_colored_bold_name() + " " + p_conf->get_elem("chat.msgs.ban") + " "
                       + p_bannick_user->get_colored_bold_name();

        if (b_reason)
          s_msg.append( " ( " + s_reason + " )");

        s_msg.append("</i><br>\n");

        p_user->msg_post(s_msg);
        if (! p_user->same_rooms(p_bannick_user) )
          p_bannick_user->msg_post(s_msg);

      }
      else
      {
        string s_msg = "<font color=\"#"
                       + p_conf->get_elem("chat.html.errorcolor")
                       + "\"><b>"
                       + s_bannick_user + "</b> "
                       + p_conf->get_elem("chat.msgs.err.notavailable")
                       + "</font><br>\n";

        p_user->msg_post( &s_msg );
      }
    }

    return 0;
  }
}

