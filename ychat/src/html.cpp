/*:*
 *: File: ./src/html.cpp
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

#ifndef HTML_CPP
#define HTML_CPP

#include <fstream>
#include "html.h"

using namespace std;

html::html( )
{
  set_name( wrap::CONF->get_elem( "httpd.templatedir" ) );
}

html::~html( )
{}

void
html::clear_cache( )
{
  clear();
  wrap::system_message( CLRHTML );
}

string
html::parse( map<string,string> &map_params )
{
  string s_file = map_params["request"];

  // check if s_file is in the container.
  string s_templ;

  // if not, read file.
  if ( ! shashmap<string>::exists( s_file ) )
  {
    string   s_path  = get_name();
    ifstream if_templ( s_path.append( s_file ).c_str(), ios::binary );

    if ( ! if_templ )
    {
      wrap::system_message( OFFFOUND + s_path );
      if (map_params["request"] == wrap::CONF->get_elem( "httpd.html.notfound" ))
        return "";

      map_params["request"] = wrap::CONF->get_elem( "httpd.html.notfound" );
      return parse( map_params );
    }

    char c_buf;
    while ( !if_templ.eof() )
    {
      if_templ.get( c_buf );
      s_templ += c_buf;
    }

    if ( map_params["content-type"].compare(0,5,"text/") == 0 )
      s_templ.erase(s_templ.end()-1);

    if_templ.close();


    // cache file.
    if (wrap::CONF->get_bool("httpd.html.cache"))
    {
      wrap::system_message( TECACHE + s_path );
      shashmap<string>::add_elem(s_templ, s_file);
    }
    else
    {
      wrap::system_message( TECACHN + s_path );
    }
  }
  else
  {
    s_templ = shashmap<string>::get_elem( s_file );
  }

  // find %%KEY%% token and substituate those.
  unsigned long pos[2];
  pos[0] = pos[1] = 0;

  for (;;)
  {
    pos[0] = s_templ.find( "%%", pos[1] );

    if ( pos[0] == string::npos )
      break;

    pos[0] += 2;
    pos[1]  = s_templ.find( "%%", pos[0] );

    if ( pos[0] == string::npos )
      break;

    // get key and val.
    string s_key = s_templ.substr( pos[0], pos[1]-pos[0] );
    string s_val = wrap::CONF->get_elem( s_key );

    // if s_val is empty use map_params.
    if ( s_val.empty() )
      s_val = map_params[ s_key ];

    // substituate key with val.
    s_templ.replace( pos[0]-2, pos[1]-pos[0]+4, s_val );

    // calculate the string displacement.
    int i_diff = s_val.length() - ( pos[1] - pos[0] + 4);

    pos[1] += 2 + i_diff;

  };

  return s_templ;
}

//<<*
void
html::online_list( user *p_user, map<string,string> &map_params )
{
  // prepare user_list.
  string s_list;

  room* p_room = p_user->get_room();

  p_room->get_user_list( s_list );

  map_params["room"] = p_room->get_name();
  map_params["topic"] = p_room->get_topic();
  map_params["userlist"] = s_list;
}
//*>>

#endif

