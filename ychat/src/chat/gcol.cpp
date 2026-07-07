/*:*
 *: File: ./src/chat/gcol.cpp
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

#ifndef GCOL_CPP
#define GCOL_CPP

#include "gcol.h"

using namespace std;

gcol::gcol()
{

  p_map_users = new shashmap<user*>;
  wrap::system_message( GARBAGE );
}

gcol::~gcol()
{
  remove_garbage();
  delete p_map_users;
}

void
gcol::add_room_to_garbage( room* p_room )
{
  vec_rooms.push_back( p_room );
  wrap::system_message( GARROOM + p_room->get_name() );
}

void
gcol::add_user_to_garbage( user* p_user )
{
  p_user->destroy_session();
  p_map_users->add_elem(p_user, tool::to_lower(p_user->get_name()));
  wrap::system_message(GARUSER + p_user->get_name());
}

bool
gcol::remove_garbage()
{
  bool b_empty;

  b_empty  = ( vec_rooms.empty() && p_map_users->size() == 0);

  if ( b_empty )
    return false;

  wrap::system_message( GARBACT );

  for ( vector<room*>::iterator iter = vec_rooms.begin();
        iter != vec_rooms.end(); iter++ )
  {
    wrap::system_message( REMROOM + (*iter)->get_name() );
    delete *iter;
  }
  vec_rooms.clear();

  // Delete only users whose stream connection is already closed. A user may
  // have been reaped (set_online(false)) while its long-lived stream frame
  // was still open; deleting the user now would leave the stream context's
  // p_user dangling (use-after-free on the later disconnect). Such users are
  // kept until their stream closes (handle_stream_read clears the fd), then
  // deleted on a later pass. Collect via run_func (the hash_map base is not
  // directly iterable from here).
  vector<user*> vec_all;
  p_map_users->run_func( collect_users_, (void*) &vec_all );

  vector<user*> vec_keep;
  for ( vector<user*>::iterator it = vec_all.begin(); it != vec_all.end(); ++it )
  {
    user* u = *it;
    if ( u->get_stream_fd() >= 0 )
      vec_keep.push_back(u);
    else
      delete_users_( u ); // clean() + delete (matches the old behaviour)
  }
  p_map_users->clear();
  for ( vector<user*>::iterator it = vec_keep.begin(); it != vec_keep.end(); ++it )
    p_map_users->add_elem( *it, tool::to_lower((*it)->get_name()) );

  return true;
}

room*
gcol::get_room_from_garbage()
{

  if ( vec_rooms.empty() )
    return NULL;

  room* p_room = vec_rooms.back();
  vec_rooms.pop_back();

  return p_room;
}

room*
gcol::get_room_from_garbage_or_new( string s_room )
{
  room* p_room = get_room_from_garbage();
  if ( p_room == NULL )
    return new room( s_room );
  p_room->set_name( s_room );
  return p_room;
}

user*
gcol::get_user_from_garbage( string s_user )
{
  user* p_user = p_map_users->get_elem(tool::to_lower(s_user));

  if ( p_user != NULL )
  {
    p_map_users->del_elem(tool::to_lower(s_user));
    p_user->set_name(s_user);
    p_user->set_online(true);
    p_user->set_invisible(false);
    p_user->set_sock(NULL);
    p_user->renew_timeout();
    wrap::system_message(GARUSE2 + p_user->get_name());
  }

  return p_user;
}

void
gcol::delete_users_( user *user_obj )
{
  wrap::system_message( REMUSER + user_obj->get_name() );
  user_obj->clean();
  delete user_obj;
}

void
gcol::collect_users_( user *user_obj, void *v_arg )
{
  static_cast<std::vector<user*>*>(v_arg)->push_back( user_obj );
}

void
gcol::lock_mutex()
{}

void
gcol::unlock_mutex()
{}

#endif
