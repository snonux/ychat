/*:*
 *: File: ./src/chat/base.h
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.4-CURRENT
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

#include "../incl.h"

#ifndef BASE_H
#define BASE_H

#include <map>
#include "../maps/nhashmap.h"

template<class type>
class base : public nhashmap<type*>
{
protected:
  void dumpit();

public:
  base();
  ~base();

  static string to_lower( string s )
  {
    string s_tmp("");
    for (int i=0;i < s.size() ;i++)
      s_tmp=s_tmp+(char)tolower(s.at(i));
    return s_tmp;
  }

  virtual type* get_elem( string s_name, bool &b_found ); // get a element.
  virtual type* get_elem( string s_name );
  virtual void  add_elem( type*   p_type );                // add a element.

  // chat::msg_post sends to all users of the system a message.
  // room::msg_post sends to all users of the room a message.
  // user::msg_post sends to the user a message.
  virtual void msg_post( string *s_msg )
  {
    nhashmap<type*>::run_func( &base<type>::msg_post_ , (void*)s_msg );
  }

  static void msg_post_( type* type_obj, void* v_arg )
  {
    type_obj -> msg_post( (string*) v_arg );
  }

  virtual void check_timeout( int* i_timeout_settings )
  {
    nhashmap<type*>::run_func( &base<type>::check_timeout_ , (void*)i_timeout_settings );
  }

  static void check_timeout_( type* type_obj, void* v_arg )
  {
    type_obj -> check_timeout( (int*) v_arg );
  }

  virtual void reconf()
  {
    nhashmap<type*>::run_func( &base<type>::reconf_ );
  }

  static void reconf_ ( type* type_obj )
  {
    type_obj -> reconf ();
  }

  void get_data( map<string,string> *p_map_string )
  {
    nhashmap<type*>::run_func( &base<type>::get_data_ , (void*)p_map_string );
  }

  static void get_data_( type* type_obj, void* v_arg )
  {
    type_obj -> get_data ( (map<string,string>*) v_arg );
  }


  // chat::get_user_list gets a list of all users of the system.
  // room::get_user_list gets a list of all users of the room.
  // user::get_user_list gets a "list" of a user <font color="usercolor">username</font>seperator
  void get_user_list( string &s_list )
  {
    container c;
    c.elem[0] = (void*) &s_list;

    nhashmap<type*>::run_func( &base<type>::get_user_list_, (void*)&c );
  }

  static void get_user_list_( type* type_obj, void* v_arg )
  {
    container *c = (container*) v_arg;
    type_obj -> get_user_list( *((string*)c->elem[0]) );
  }
};

#include "base.tmpl"

#endif
