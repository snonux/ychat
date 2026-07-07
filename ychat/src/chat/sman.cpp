/*:*
 *: File: ./src/chat/sman.cpp
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

#ifndef SMAN_CPP
#define SMAN_CPP

#include "sman.h"
#include "../tool/tool.h"
#include "../maps/mtools.h"
#include "../contrib/crypt/md5.h"

#include <fstream>
#include <unistd.h>
#include <ctime>

// Build one candidate session ID of i_len chars drawn from s_valid, using a
// strong random source (/dev/urandom) with a one-time rand() fallback.
// NOTE: deliberately does NOT re-seed srand() with time(0) on every call —
// the old code did that, which (a) made IDs predictable and (b) made two
// logins in the same second produce identical IDs; the collision retry then
// re-seeded with the same time and recursed forever -> stack overflow.
static string
gen_session_candidate( int i_len, const string &s_valid )
{
  static ifstream urandom( "/dev/urandom", ios::binary );
  static bool b_seeded = false;

  size_t n = s_valid.length();
  string s;
  s.reserve( i_len );

  for ( int i = 0; i < i_len; i++ )
  {
    unsigned char b;
    if ( urandom.is_open() && urandom.read( (char*)&b, 1 ) )
      s += s_valid[ b % n ];
    else
    {
      // A failed read sets failbit and would stick forever; clear it so a
      // transient failure can self-heal on the next byte (if urandom is
      // genuinely unavailable we keep falling back to rand()).
      urandom.clear();
      if ( ! b_seeded )
      {
        srand( (unsigned) time(0) ^ ( (unsigned) getpid() << 8 ) );
        b_seeded = true;
      }
      s += s_valid[ rand() % n ];
    }
  }

  return s;
}

sman::sman()
{
  i_continous_session_count = i_session_count = 0;
}

sman::~sman()
{
  // Delete each session object of the shashmap!
  shashmap<sess*>::run_func( mtools<sess*>::delete_obj );
}

string sman::generate_id( int i_len )
{
  string s_valid = wrap::CONF->get_elem("chat.session.validchars");
  if ( s_valid.empty() || i_len <= 0 )
    return ""; // misconfigured: nothing to build an ID from

  // Bounded collision retry. With /dev/urandom the collision probability is
  // negligible, but a bound prevents any pathological case from recursing
  // forever (the old code recursed unboundedly and crashed on same-second
  // collisions).
  for ( int i_try = 0; i_try < 8; i_try++ )
  {
    string s_ret = gen_session_candidate( i_len, s_valid );

    if ( wrap::CONF->get_elem("chat.session.md5hash") == "true" )
    {
      string s_salt = wrap::CONF->get_elem("chat.session.md5salt");
      string s_hash(md5::MD5Crypt(s_ret.c_str(), s_salt.c_str()));
      s_ret.append(s_hash.substr(s_ret.find(s_salt) + s_salt.length() + 3));
    }

    // Prove, if the TempID already exists
    if ( ! get_elem(s_ret) )
      return s_ret;

    wrap::system_message(SESSEXI);
  }

  // Astronomically unlikely with a strong random source. Return a final
  // candidate only if it does NOT collide (so we never overwrite/leak an
  // existing session); otherwise bail with an empty ID (login degrades
  // gracefully instead of crashing or leaking).
  string s_last = gen_session_candidate( i_len, s_valid );
  if ( ! get_elem(s_last) )
    return s_last;

  wrap::system_message("SMAN: gave up finding a unique session id");
  return "";
}

sess *sman::create_session( )
{
  string s_tmpid = generate_id( tool::string2int( wrap::CONF->get_elem( "chat.session.length" ) ) );

  sess* p_sess = new sess( s_tmpid );

  i_session_count++;
  wrap::system_message(string(SESSIOC) + "(" +
                       tool::int2string(++i_continous_session_count) + "," +
                       tool::long2string((long)p_sess) + ")" );


  //????
  add_elem( p_sess, s_tmpid );

  return p_sess;
}

sess *sman::get_session( string s_id )
{
  return get_elem( s_id );
}

void
sman::destroy_session( string s_id )
{
  sess* p_sess = get_elem(s_id);

  i_session_count--;

  wrap::system_message(string(SESSIOD) + "(" +
                       tool::int2string(i_continous_session_count) + "," +
                       tool::long2string((long)p_sess) + ")" );


  del_elem(s_id);
  delete p_sess;
}

int
sman::get_session_count()
{
  int i_ret = i_session_count;
  return i_ret;
}

/*
void
sman::dump() {
  shashmap<sess*>::dump();
  cout << "BLA" << endl;
}
*/

#endif
