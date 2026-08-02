/*:*
 *: File: ./src/chat/gcol.h
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

// garbage collector
#include "../incl.h"

#ifndef GCOL_H
#define GCOL_H

#include <vector>
#include "room.h"
#include "user.h"
#include "../maps/shashmap.h"

using namespace std;

class gcol
{
private:
  vector<room*> vec_rooms;
  shashmap<user*>* p_map_users;


  static void delete_users_( user* user_obj  );
  static void collect_users_( user* user_obj, void* v_arg );

public:
  gcol();
  ~gcol();

  bool remove_garbage();
  void add_room_to_garbage( room* p_room );
  void add_user_to_garbage( user* p_user );
  virtual room* get_room_from_garbage();
  virtual room* get_room_from_garbage_or_new( string s_room );
  user* get_user_from_garbage( string s_user );
  virtual void lock_mutex();
  virtual void unlock_mutex();
};

#endif
