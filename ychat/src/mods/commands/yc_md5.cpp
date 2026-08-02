/*:*
 *: File: ./src/mods/commands/yc_md5.cpp
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

#include "../../incl.h"
#include "../../chat/user.h"
#include "../../contrib/crypt/md5.h"

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
    conf* p_conf = (conf*) ((dynamic_wrap*)c->elem[3])->CONF;

    if (params->size() >= 2)
    {
      vector<string>::iterator iter;
      string s_string = "";
      string s_salt   = "";

      for (iter = params->begin(); iter+1 != params->end(); ++iter)
      {
        s_string.append(*iter);
      }

      s_salt = *iter;

      string s_msg = "Crypt: <b>" + s_string + "</b>, Salt: <b>"
                     + s_salt + "</b> = <b>" +
                     + md5::MD5Crypt(s_string.c_str(), s_salt.c_str())
                     + "</b><br>\n";
      p_user->msg_post( &s_msg );
    }
    else
    {
      string s_msg = "<font color=\"#"
                     + p_conf->get_elem("chat.html.errorcolor")
                     + "\"> "
                     + p_conf->get_elem("ERR_WRONG_COMMAND_USAGE")
                     + "</font><br>\n";
      p_user->msg_post( &s_msg );

    }

    return 0;
  }
}

