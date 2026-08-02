/*:*
 *: File: ./src/mods/html/yc_colors.cpp
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

/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

extern "C"
{
  int extern_function(void *v_arg)
  {
    container* c          = (container*) v_arg;
    dynamic_wrap* p_wrap = (dynamic_wrap*) c->elem[0];
    conf*      p_conf    = (conf*) p_wrap->CONF;
    string*    p_col1     = &(*((map<string,string>*) c->elem[1]))["col1"];
    string*    p_col2     = &(*((map<string,string>*) c->elem[1]))["col2"];
    string*    p_flag     = &(*((map<string,string>*) c->elem[1]))["flag"];
    user*      p_user     = (user*) c->elem[2];

    p_col1->erase(0);
    p_col2->erase(0);

    p_col1->append( p_user->get_col1() );
    p_col2->append( p_user->get_col2() );

    if ( *p_flag == "submit" )
    {
      string*  p_msgs  = &(*((map<string,string>*) c->elem[1])
                          )["msgs"];
      p_msgs->append( p_conf->get_elem( "chat.msgs.optionschanged" ) );
    }

    return 0;
  }
}

