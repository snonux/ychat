/*:*
 *: File: ./src/mods/html/yc_help.cpp
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

/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

extern "C"
{
  int extern_function(void *v_arg)
  {
    container* c         = (container*) v_arg;
    dynamic_wrap* p_wrap = (dynamic_wrap*) c->elem[0];
    user* 	   p_user    = (user*) c->elem[2];

    modl*      p_modl    = (modl*) p_wrap->MODL;
    conf*      p_conf    = (conf*) p_wrap->CONF;
    chat*      p_chat    = (chat*) p_wrap->CHAT;
    string*    s_content = &(*((map<string,string>*) c->elem[1]))["content"];

    vector<string>* p_vec_keys = p_modl->get_mod_vector();

    sort(p_vec_keys->begin(), p_vec_keys->end());
    vector<string>::iterator p_vec_keys_iter = p_vec_keys->begin();

    while ( p_vec_keys_iter != p_vec_keys->end() )
    {

      if ( p_vec_keys_iter->find( "mods/commands/", 0 ) != string::npos )
      {

        string s_command_name = p_vec_keys_iter->substr(17);
        s_command_name.erase( s_command_name.find( ".so" ) );

        if ( !p_chat->get_command_disabled( s_command_name ) &&
             p_chat->get_command_status( s_command_name ) >= p_user->get_status())
        {
          s_content->append( "<table bordercolor=\"1\">\n" );
          s_content->append( "<tr><td><font color=\"#FFFFFF\"><b>/" + s_command_name + "</b></font></td></tr>\n" );
          s_content->append( "<tr><td>" + p_conf->get_elem("chat.msgs.help." + s_command_name) + "</td></tr>\n" );
          s_content->append( "</table>\n<br />\n" );
        }
      }
      p_vec_keys_iter++;
    }
    return 0;
  }
}

