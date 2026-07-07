/*:*
 *: File: ./src/chat/perm.cpp
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

#ifndef PERM_CPP
#define PERM_CPP

#include "perm.h"
#include "../maps/mtools.h"

using namespace std;

perm::perm()
{
  p_command_status = new shashmap< valwrap<int>* >;
  p_command_disabled = new shashmap< valwrap<bool>* >;

  set_standard_command_permissions();
}

perm::~perm()
{
  p_command_status->run_func( &mtools< valwrap<int>* >::delete_obj );
  p_command_disabled->run_func( &mtools< valwrap<bool>* >::delete_obj );

  delete p_command_status;
  delete p_command_disabled;
}

int
perm::get_command_status( string s_command )
{
  if ( p_command_status->exists( s_command ) )
    return p_command_status->get_elem( s_command )->val;

  return 0;
}

void
perm::set_command_status( string s_command, int i_int )
{
  valwrap<int>* p_val = p_command_status->get_set_elem( new valwrap<int>(i_int), s_command );

  if ( p_val )
    delete p_val;
}

bool
perm::get_command_disabled( string s_command )
{
  if ( p_command_disabled->exists( s_command ) )
    return p_command_disabled->get_elem( s_command )->val;
  return 0;
}

void
perm::set_command_disabled( string s_command, bool b_bool )
{
  valwrap<bool>* p_val = p_command_disabled->get_set_elem( new valwrap<bool>(b_bool), s_command );

  if ( p_val )
    delete p_val;
}

void
perm::set_standard_command_permissions()
{
  wrap::system_message(PERMSTD);

  vector<string>* p_vec_keys = wrap::CONF->get_key_vector();

  for (vector<string>::iterator iter = p_vec_keys->
                                       begin();
       iter != p_vec_keys->end();
       iter++ )
  {
    if ( iter->length() > 17 && iter->compare( 0, 16, "chat.permissions" ) == 0 )
    {
      valwrap<int>* p_val = new valwrap<int>( tool::string2int( wrap::CONF->get_elem(*iter) ) );
      p_command_status->add_elem( p_val, iter->substr(17) );
    }
  }

  delete p_vec_keys;
}

#endif
