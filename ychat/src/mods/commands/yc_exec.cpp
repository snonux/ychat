/*:*
 *: File: ./src/mods/commands/yc_exec.cpp
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
#include "../../chat/room.h"
#include "../../chat/user.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

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

    string s_command;
    string s_msg = p_timr->get_time();
    string s_output;

    vector<string>::iterator iter;
    for ( iter = params->begin(); iter != params->end(); iter++ )
      s_command.append( " " + *iter );

    s_command.append(" 2>error.log");

    s_msg.append( " " + s_command + "<br>\n" );

    p_user->msg_post( &s_msg );

    FILE *file;
    char buffer[READBUF];

    if ( (file=popen(s_command.c_str(), "r")) == NULL )
    {
      s_output = "<font color=\"#"
                 + p_conf->get_elem("chat.html.errorcolor")
                 + p_conf->get_elem("chat.msgs.err.execcommand")
                 + "</font><br>\n";
      p_user->msg_post( &s_output );
    }
    else
    {
      s_output.append("<hr>\n");
      while (true)
      {
        if (fgets(buffer, READBUF, file) == NULL)
          break;

        s_output.append( string(buffer) + "<br>\n" );
      }

      p_user->msg_post( &s_output );
      pclose(file);
    }

    return 0;
  }
}

