/*:*
 *: File: ./src/chat/chat.h
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

// class chat declaration.

class chat;

#ifndef CHAT_H
#define CHAT_H

#include <vector>
#include "../incl.h"
#include "base.h"
#include "perm.h"
#include "room.h"
#include "user.h"
#include "sess.h"
#include "../tool/tool.h"
#include "../maps/shashmap.h"

using namespace std;

#undef chat
class chat : public base<room>, public perm
{
private:
  map<string,string> map_replace_strings;
  vector<string> vec_replace_keys;
  shashmap<string> map_banned_nicks;
  shashmap<string, unsigned, self_hash<unsigned>, equals_allocator<unsigned> > map_banned_ips;

  void dumpit();

public:
  virtual room* get_room( string s_name )
  {
    bool b_bool;
    return get_room( s_name, b_bool );
  }

  virtual room* get_room( string &s_name, bool &b_found )
  {
    return static_cast<room*>( get_elem( s_name, b_found ) );
  }

  void del_elem( string s_name )
  {
    base<room>::del_elem( s_name );
  }

  // public methods:
  explicit chat();  // a standard constructor.
  ~chat();          // destructor.

  // get the object of a specific user.
  user* get_user( string &s_nick );
  user* get_user( string &s_nick, bool &b_found );
  static void get_user_( room* room_obj, void *v_arg   );

  // will be called every time a user tries to login.
  void login( map<string,string> &map_params );

  // will be called if a user posts a message.
  void post ( user* u_user, map<string,string> &map_params );

  void reconf();
  void string_replacer(string *p_msg);
  string ban_nick(string &s_nick, string s_reason);
  string unban_nick(string &s_nick);
  shashmap<string>* get_map_banned_nicks();
};
#endif
