/*:*
 *: File: ./src/chat/user.h
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

#include "../incl.h"

#ifndef USER_H
#define USER_H

#include "../name.h"
#include "../time/timo.h"
#include "../memb/memb.h"
#include "../monitor/dump.h"

using namespace std;

class room;
//class sess;

class user :
      public name,
      public timo,
      public dumpable,
      public memb_base
{
private:

  // private members:
  _socket *p_sock;
  int   i_stream_fd; // long-lived stream connection fd (-1 = none)
  bool  b_stream_ready; // initial HTTP response sent; ok to push messages
  string s_msg;
  bool b_online; // true if user is online.
  bool b_has_sess; // true if user already has a session!
  bool b_is_reg; // true if user is registered
  bool b_is_gag; // true if user is gagged
  bool b_away;   // true if user is away.
  bool b_fake;   // true if user hides his status logo (does not work for guest)
  bool b_invisible;   // true if user hides his status logo (does not work for guest)
  bool b_set_changed_data; // Only set change data if required!

  int i_status;   // user's rang ( see enum rang @ globals.h ).
  int i_old_status;   // user's previous status.
  int i_flood_messages; // user's message posts (needed for flood protection, does not need to be syncronized)
  time_t t_flood_time; // user's time count (needed for flood protection, does not need to be syncronized)
  long l_messages_recv;

  string s_tmpid;
  string s_agnt;   // user's http user agent.
  string s_away;   // user's last away message.
  string s_col1;   // user's nick color.
  string s_col2;   // user's text color.
  string s_email;  // user's email addres
  string s_pass;   // password
  room*  p_room;   // pointer to the user's room.

  hashmap<string> map_changed_data; // Needed to tell yChat which data to change after user is removed!

  void initialize();
  void set_changed_data( string s_varname, string s_value );
  void dumpit();

public:

  user();
  user(string s_name);
  ~user();

  void clean();
  void destroy_session();

  // gets specific data of this user und stores it in
  // (*p_map<string,string>)["nick"]. this method will be used
  // every time data has to be got from every user of a room
  // or even of the system.
  void get_data( map<string,string> *p_map_data );

  string get_colored_name();
  string get_colored_bold_name()
  {
    return "<b>" + get_colored_name() + "</b>";
  }
  void msg_clear()
  {
    s_msg.clear();
  }
  bool get_online();
  bool get_fake();
  bool get_invisible();
  bool get_has_sess();
  bool get_is_reg();
  bool get_is_gag();
  void set_online( bool b_online );
  void set_sock(_socket *p_sock);
  // Stream (long-lived chat-display) connection for this user.
  void set_stream_fd(int i_fd);
  int  get_stream_fd();
  void set_stream_ready(bool b) { b_stream_ready = b; }
  void flush_stream();   // write buffered s_msg to the stream fd
  void clear_stream();   // drop stream state (disconnect/logout)
  void set_fake( bool b_fake );
  void set_invisible( bool b_invisible );
  void set_has_sess( bool b_has_sess );
  void set_is_reg( bool b_is_reg );
  void set_is_gag( bool b_is_gag );
  void set_changed_data_on()
  {
    b_set_changed_data = 1;
  }
  bool get_away( );
  string get_away_msg( );
  void set_away( bool b_away, string s_away );
  void set_away( bool b_away );
  room* get_room();
  void set_p_room( room* p_room );
  //  void  set_sess(sess** p_sess);
  string get_pass();
  string get_col1();
  string get_col2();
  string get_email();
  string get_tmpid();
  void set_tmpid( string s_tmpid );
  void set_pass( string s_col1 );
  void set_col1( string s_col1 );
  void set_col2( string s_col2 );
  void set_email( string s_email );
  int  get_status( );
  void set_status( int i_status );
  void set_messages_recv( long l )
  {
    l_messages_recv = l;
  }
  void post_action_msg(string s_msgkey);
  void check_timeout( int* i_idle_timeout );
  void renew_timeout();

  // executes a command.
  void command( string &s_command );


  void msg_post( string s_msg )
  {
    msg_post( &s_msg );
  }
  void msg_post( string *p_msg );
  void get_user_list( string &s_list );
  void check_restore_away();
  void reconf();
  bool same_rooms(user *p_user);
  string make_colors(string s_msg);
};

#endif
