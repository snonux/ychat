/*:*
 *: File: ./src/mods/html/yc_register.cpp
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

#include "../../incl.h"

/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

extern "C"
{
  int extern_function(void *v_arg)
  {
#ifdef DATABASE
    container* c          = (container*) v_arg;
    dynamic_wrap* p_wrap  = (dynamic_wrap*) c->elem[0];
    conf*      p_conf     = (conf*) p_wrap->CONF;
    ychatdb*   p_data     = (ychatdb*) p_wrap->DATA;
    string*    p_msgs     = &(*((map<string,string>*) c->elem[1]))["INFO"];
    string*    p_nick     = &(*((map<string,string>*) c->elem[1]))["nick"];
    string*    p_email    = &(*((map<string,string>*) c->elem[1]))["email"];
    string*    p_pass     = &(*((map<string,string>*) c->elem[1]))["pass"];
    string*    p_pass2    = &(*((map<string,string>*) c->elem[1]))["pass2"];
    string*    p_request  = &(*((map<string,string>*) c->elem[1]))["request"];
    user*      p_user     = (user*) c->elem[2];
    string     s_nick     = *p_nick;


    p_user->set_name( "!" + s_nick );
    p_user->set_has_sess( false );
    *p_nick = tool::to_lower(*p_nick);

    if ( *p_pass != *p_pass2 )
    {
      p_msgs->append( p_conf->get_elem( "chat.msgs.err.registerpassword" ) )
      ;
    }

    // prove if the nick is alphanumeric:
    else if ( ! tool::is_alpha_numeric( *p_nick ) )
    {
      p_msgs->append( p_conf->get_elem( "chat.msgs.err.alpnum" ) )
      ;
    }

    // prove if the nick is too long:
    else if ( p_nick->length() > tool::string2int( p_conf->get_elem("chat.maxlength.username")
                                                 ) )
    {
      p_msgs->append( p_conf->get_elem( "chat.msgs.err.nicklength" ) );
    }
    else if ( p_pass->length() > tool::string2int( p_conf->get_elem("chat.maxlength.password")
                                                 ) )
    {
      p_msgs->append( p_conf->get_elem( "chat.msgs.err.passlength" ) );
    }
    else if ( p_email->length() > tool::string2int( p_conf->get_elem("chat.maxlength.emailaddress")
                                                  ) )
    {
      p_msgs->append( p_conf->get_elem( "chat.msgs.err.emaillength" ) );
    }
    else
    {
      hashmap<string> result_map = p_data->select_user_data( *p_nick, "selectnick" );
      if (result_map["nick"] == *p_nick)
      {
        p_msgs->append( p_conf->get_elem( "chat.msgs.err.registerexists" ) );
      }
      else
      {
        p_user->set_name( s_nick );
        p_msgs->append( p_conf->get_elem( "chat.msgs.registernick" ) );
        p_request->replace( 0, p_request->length(), p_conf->get_elem("httpd.startsite") );

        map<string,string> map_insert = *((map<string,string>*) c->elem[1]);
        map_insert["password"] = *p_pass;
        map_insert["color1"] = p_conf->get_elem("chat.html.user.color1");
        map_insert["color2"] = p_conf->get_elem("chat.html.user.color2");
        map_insert["registerdate"] = tool::int2string((int)tool::unixtime());

        p_user->set_pass( map_insert["password"] );
        p_user->set_email( map_insert["email"] );
        p_user->set_col1( map_insert["color1"] );
        p_user->set_col2( map_insert["color2"] );
        p_user->set_is_reg( 1 );
        p_user->set_status( tool::string2int(p_conf->get_elem("chat.defaultrang") ) );

        p_data->insert_user_data( *p_nick, "registernick", map_insert );
      }
    }

    return 0;
#endif

  }
}

