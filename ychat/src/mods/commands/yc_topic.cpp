/*:*
 *: File: ./src/mods/commands/yc_topic.cpp
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

#include "../../tool/tool.h"
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
    timr* p_timr = (timr*) ((dynamic_wrap*)c->elem[3])->TIMR;
    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;
    vector<string> *params = (vector<string>*) c->elem[2];	// param array

    string s_msg;
    string s_topic;
    string s_col;

    room* p_room = p_user->get_room();

    if ( ! params->empty() )
    {
      vector<string>::iterator iter;

      s_col = "<font color=\"" + p_user->get_col1() + "\">";

      s_msg = p_timr->get_time()
              + " "
              + p_user->get_colored_bold_name()
              + " "
              + p_conf->get_elem("chat.msgs.topic")
              + s_col;

      for ( iter = params->begin(); iter != params->end(); iter++ )
      {
        s_msg.append( " " + *iter );
        s_topic.append( *iter + " " );
      }
      s_msg.append( "</font>" );

      int i_topic_length = tool::string2int(p_conf->get_elem("chat.maxlength.topic"));
      if ( s_topic.length() > i_topic_length )
      {
        s_topic = s_topic.substr(0, i_topic_length );
        s_topic.append(" ... ");

        string s_private_msg = "<font color=#" + p_conf->get_elem("chat.html.errorcolor")
                               + ">" + p_conf->get_elem( "chat.msgs.topiccut" ) + "</font><br>";

        p_user->msg_post ( &s_private_msg );
      }

      s_topic = "( " + s_topic + ")";
      p_room->set_topic( s_topic, p_user->get_col1() );
    }
    else
    {
      s_msg = p_timr->get_time()
              + " "
              + p_user->get_colored_bold_name()
              + " "
              + p_conf->get_elem("chat.msgs.topicdelete");

      p_room->set_topic( "" );
    }

    s_msg.append( "<script confuage='JavaScript'>parent.online.location.reload();</script><br>\n" );

    p_room->msg_post ( &s_msg  );

    return 0;
  }
}

