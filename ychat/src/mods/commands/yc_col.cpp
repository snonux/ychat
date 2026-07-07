/*:*
 *: File: ./src/mods/commands/yc_col.cpp
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

#include <iostream>
#include "../../wrap.h"
#include "../../chat/room.h"
#include "../../chat/user.h"
#include "../../tool/tool.h"

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


    user *p_user = (user*)c->elem[1];		// the corresponding user
    vector<string> *params= (vector<string>*) c->elem[2];	// param array

    string s_color;
    string s_color2;

    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;
    timr* p_timr = (timr*) ((dynamic_wrap*)c->elem[3])->TIMR;
    if ( params->empty() )
    {
      s_color  = p_conf->get_elem( "chat.html.user.color1" );
      s_color2 = p_conf->get_elem( "chat.html.user.color2" );
    }
    else
    {
      s_color = (string) params->front();
      params->erase( params->begin() );
      if ( ! params->empty() )
        s_color2 = (string) params->front();
      else
        s_color2 = p_conf->get_elem( "chat.html.user.color1" );
    }

    s_color  = tool::to_lower( s_color  );
    s_color2 = tool::to_lower( s_color2 );

    if ( valid_color(s_color) != 1  )
    {
      string *answerstring=new string(s_color + " is not a valid color.<br>\n");
      p_user->msg_post( answerstring );
    }
    else if ( valid_color(s_color2) != 1  )
    {
      string *answerstring=new string( s_color2 + " is not a valid color.<br>\n");
      p_user->msg_post( answerstring );
    }
    else
    {
      string *answerstring=new string(p_timr->get_time() + " " + p_user->get_colored_bold_name()+ " changes color to <font color=\"#"
                                      + s_color + "\">" + s_color + "</font> <font color=\"#"
                                      + s_color2 + "\">" + s_color2 + "</font><br>\n");
      p_user->get_room()->msg_post( answerstring );
      p_user->set_col1(s_color);
      p_user->set_col2(s_color2);
      p_user->get_room()->reload_onlineframe();
    }

    return 0;
  }
  int valid_color( string s_color )
  {

    if (s_color.size()!=6)
      return 0;
    string valid="abcdef0123456789";
    for (int i=0;i<s_color.size();i++)
    {
      string s_char=s_color.substr(i,1);
      if (valid.find(s_char)==string::npos)
        return 0;
    }

    return 1;
  }

}

