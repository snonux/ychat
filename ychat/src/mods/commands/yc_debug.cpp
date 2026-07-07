/*:*
 *: File: ./src/mods/commands/yc_debug.cpp
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
/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

// THIS MODULE IS FOR DEBUGGING PURPOSE ONLY, IT DOESNT HAVE ANY OTHER USE!

extern "C"
{
  int valid_color( string );

  int extern_function(void *v_arg)
  {
    container *c=(container *)v_arg;

    user *p_user = (user*) c->elem[1];		// the corresponding user
    vector<string> *params = (vector<string>*) c->elem[2];	// param array
    chat* p_chat = (chat*) ((dynamic_wrap*)c->elem[3])->CHAT;
    sman* p_sman = (sman*) ((dynamic_wrap*)c->elem[3])->SMAN;

    if ( ! params->empty() )
    {
      vector<string>::iterator iter = params->begin();
      string s_test_user( *iter );
      bool b_found;
      user* p_whisper_user = p_chat->get_user( s_test_user, b_found );
      sess *p_sess = p_sman->get_elem(p_user->get_tmpid());

      string s_msg = "DEBUG-A: " + tool::long2string(reinterpret_cast<long>(p_sess->get_user())) + "<br>";
      p_user->msg_post( &s_msg );

      //p_user->debug();
      s_msg = "DEBUG-B: " + tool::long2string(reinterpret_cast<long>(p_sess->get_user())) + "<br>";
      p_user->msg_post( &s_msg );
    }

    return 0;
  }
}

