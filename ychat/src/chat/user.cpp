/*:*
 *: File: ./src/chat/user.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.0-CURRENT
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

#ifndef USER_CPP
#define USER_CPP

#include "user.h"
#include "../tool/tool.h"

using namespace std;

#include <errno.h>
#include <unistd.h>
#include <cstring>

user::user() : name( "" )
{
  initialize();
}

user::user( string s_name ) : name( s_name )
{
  initialize();
}

user::~user()
{
  // The stream fd is owned by its context, not by the user.
}

void
user::initialize()
{
  time(&t_flood_time);
  init_strings(wrap::CONF->get_vector("chat.fields.userstrings"));
  init_ints(wrap::CONF->get_vector("chat.fields.userints"));
  init_bools(wrap::CONF->get_vector("chat.fields.userbools"));

  this -> l_messages_recv = 0;
  this -> p_sock = NULL;
  this -> i_stream_fd = -1;
  this -> b_stream_ready = false;
  this -> p_room = NULL;
  this -> s_msg = "";
  this -> b_is_reg = false;
  this -> b_is_gag = false;
  this -> b_set_changed_data = false;
  this -> b_away = false;
  this -> b_fake = false;
  this -> b_invisible = false;
  this -> s_col1 = wrap::CONF->get_elem( "chat.html.user.color1" );
  this -> s_col2 = wrap::CONF->get_elem( "chat.html.user.color2" );

  renew_timeout();
}

void
user::clean()
{
  destroy_session();
  set_fake(false);
  set_invisible(false);
  set_away(false, "");
  set_sock(NULL);
  clear_stream();
}

void
user::destroy_session()
{
  if ( !get_has_sess() )
    return;

#ifdef DATABASE
  // Store all changed data into the mysql table if this user is registered:
  if ( b_is_reg )
    wrap::DATA->update_user_data( get_name(), "savechangednick",
                                  map_changed_data );
#endif

  set_has_sess(false);
  wrap::SMAN->destroy_session(get_tmpid());
  //wrap::system_message(tool::int2string(reinterpret_cast<int>(*p_sess)));
  set_tmpid("");
}

string
user::get_colored_name()
{
  return "<font color=\"#" + get_col1() + "\">" + get_name() + "</font>";
}

void
user::get_data( map<string,string> *p_map_data )
{
  string s_req = (*p_map_data)["!get"];

  // get the nick and the color of the user.
  if ( s_req == "nick" )
    (*p_map_data)[get_name()] = get_col1();
}

bool
user::get_online( )
{
  return b_online;
}

bool
user::get_fake( )
{
  return b_fake;
}

bool
user::get_invisible( )
{
  return b_invisible;
}

bool
user::get_has_sess( )
{
  return b_has_sess;
}

void
user::set_has_sess(bool b_has_sess)
{
  this->b_has_sess = b_has_sess;
}

bool
user::get_is_reg( )
{
  return b_is_reg;
}

bool
user::get_is_gag( )
{
  return b_is_gag;
}

void
user::set_is_reg( bool b_is_reg )
{
  this -> b_is_reg = b_is_reg;
}

void
user::set_is_gag( bool b_is_gag )
{
  this -> b_is_gag = b_is_gag;
}

void
user::set_online( bool b_online )
{
  if (this->b_online == b_online)
    return;

  this -> b_online = b_online;
  if (!b_online)
  {
    cout << "SETTING OFFLINE" << endl;
    // remove the user from its room.
    string s_user(get_name());
    string s_user_lowercase(get_lowercase_name());

    room* p_room = get_room();
    p_room->del_elem(s_user_lowercase);

    // post the room that the user has left the chat.
    string s_msg = wrap::TIMR->get_time() + " "
                   + get_colored_bold_name()
                   + wrap::CONF->get_elem( "chat.msgs.userleaveschat" )
                   + "<br>\n";

    p_room->msg_post( &s_msg );
    p_room->reload_onlineframe();

#ifdef VERBOSE
    cout << REMUSER << s_user << endl;
#endif

    wrap::GCOL->add_user_to_garbage(this);
  }
}

void
user::set_fake( bool b_fake )
{
  this -> b_fake = b_fake;
}

void
user::set_invisible( bool b_invisible )
{
  this -> b_invisible = b_invisible;
}

bool
user::get_away()
{
  bool b_ret;
  b_ret = b_away;
  return b_ret;
}

string
user::get_away_msg()
{
  string s_ret;
  s_ret = s_away;
  return s_ret;
}

void
user::set_away( bool b_away, string s_away )
{
  this -> b_away = b_away;
  this -> s_away = s_away;
}

void
user::set_away( bool b_away )
{
  this -> b_away = b_away;
}

room*
user::get_room( )
{
  room* p_return;
  p_return = p_room;
  return p_return;
}

void
user::set_p_room( room* p_room )
{
  this -> p_room = p_room;
}

/*
void
user::set_sess( sess** p_sess )
{
  this -> p_sess = p_sess;
}
*/

string
user::get_pass()
{
  string s_ret;
  s_ret = s_pass;
  return s_ret;
}

string
user::get_col1()
{
  string s_ret;
  s_ret = s_col1;
  return s_ret;
}

string
user::get_col2()
{
  string s_ret;
  s_ret = s_col2;
  return s_ret;
}

string
user::get_email()
{
  string s_ret;
  s_ret = s_email;
  return s_ret;
}

string
user::get_tmpid()
{
  string s_ret;
  s_ret = s_tmpid;
  return s_ret;
}

void
user::set_tmpid    ( string s_tmpid   )
{
  this -> s_tmpid = s_tmpid;
}

void
user::set_pass  ( string s_pass )
{
  set_changed_data( "password", s_pass );
  this -> s_pass = s_pass;
}

void
user::set_col1  ( string s_col1 )
{
  set_changed_data( "color1", s_col1 );
  this -> s_col1 = s_col1;
}

void
user::set_col2  ( string s_col2 )
{
  set_changed_data( "color2", s_col2 );
  this -> s_col2 = s_col2;
}

void
user::set_email ( string s_email )
{
  set_changed_data( "email", s_email );
  this -> s_email = s_email;
}

int
user::get_status  ( )
{
  int r_ret;
  r_ret = i_status;
  return  r_ret;
}

void
user::set_status  ( int   i_status )
{
  set_changed_data( "status", tool::int2string(i_status));
  i_old_status = this -> i_status;
  this -> i_status = i_status;
}

void
user::set_changed_data( string s_varname, string s_value )
{
  if ( b_set_changed_data )
  {
    map_changed_data[s_varname] = s_value;
  }
}

void
user::command( string &s_command )
{
  check_restore_away();

  unsigned long pos = s_command.find( "/" );
  unsigned long pos2 = s_command.find( " " );
  if ( pos != (unsigned long) string::npos )
  {
    s_command.replace( pos, 1, "" );
  }
  else
    return;

  if (pos2 == string::npos)
    pos2 = s_command.size() + 1;

  string s_mod( wrap::CONF->get_elem("httpd.modules.commandsdir") + "yc_" );
  string s_command2 = s_command.substr(0, pos2-1);
  s_mod.append( s_command2  ).append( ".so" );

  dynmod *mod = wrap::MODL->get_module( s_mod, get_name() );

  if ( mod == NULL ||
       wrap::CHAT->get_command_disabled( s_command2 ) ||
       get_status() > wrap::CHAT->get_command_status( s_command2 ) )
  {
    string s_msg =  "<font color=\"" + wrap::CONF->get_elem("chat.html.errorcolor") + "\">"
                    + wrap::CONF->get_elem( "chat.msgs.err.findingcommand" )
                    + "</font>\n";
    msg_post( &s_msg );
    return;
  }

  vector<string> params;

  // execute the module.
  if (s_command.find(" ") != string::npos)
  {
    s_command = s_command.substr(s_command2.size()+1);
    pos = s_command.find(" ");
    pos2 = 0;

    while (pos != string::npos)
    {
      string sParam = s_command.substr(pos2, pos - pos2);
      params.push_back(sParam);
      pos2 = pos + 1;
      pos = s_command.find(" ", pos2);
    }

    if (pos2 < s_command.size())
    {
      params.push_back(s_command.substr(pos2, s_command.size()-pos2));
    }

  }

  container* c = new container;
  //c->elem[0]=(void*) unreserved
  c->elem[1] = (void*) this;
  c->elem[2] = (void*) &params;
  c->elem[3] = (void*) wrap::WRAP;

  ( *(mod->the_func) ) ( static_cast<void*>(c) );

  delete c;
}

void
user::set_sock(_socket* p_sock)
{
  this->p_sock = p_sock;
}

void
user::set_stream_fd(int i_fd)
{
  // The context owns the fd and closes it on disconnect; the user only
  // stores the number so msg_post can write to it. -1 = no stream.
  i_stream_fd = i_fd;
}

int
user::get_stream_fd()
{
  return i_stream_fd;
}

// Write a chat message to this user's open stream connection.
// Single-threaded (libevent loop), so no locking needed. Messages are
// buffered in s_msg until the initial HTTP response has been sent
// (b_stream_ready), then flushed. Best-effort non-blocking write; anything
// that can't be written now stays buffered for the next flush. On a hard
// write error the peer is gone: drop our reference (the stream context's
// read event will reap it and close the fd) and mark the user offline.
void
user::msg_post( string *p_msg )
{
  ++l_messages_recv;

  if ( i_stream_fd < 0 )
    return; // no open stream connection; nothing to deliver to

  s_msg.append(*p_msg);
  if ( ! b_stream_ready )
    return; // initial response not yet sent; keep buffered

  flush_stream();
}

void
user::flush_stream()
{
  while ( ! s_msg.empty() && i_stream_fd >= 0 )
  {
    ssize_t n = write( i_stream_fd, s_msg.data(), s_msg.size() );
    if ( n > 0 )
    {
      s_msg.erase( 0, n );
      continue;
    }
    if ( n == 0 )
      break;

    if ( errno == EAGAIN || errno == EINTR )
      break; // socket buffer full; keep buffered for later

    // Hard error (EPIPE, ECONNRESET, ...): peer gone.
    clear_stream();
    set_online( false );
    return;
  }
}

void
user::clear_stream()
{
  i_stream_fd = -1;
  b_stream_ready = false;
  s_msg.clear();
}
/*
    void
    sock::handle_client_write(int i_fd, short event, void *p_arg) {
        static int i_char_size = sizeof(char);

        context *p_context = static_cast<context*>(p_arg);
        string *p_response = p_context->p_response;

        if (-1 == write(i_fd, p_response->c_str(), p_response->length()*i_char_size))
        {
            switch (errno) {
            case EAGAIN:
            case EINTR:
                event_add(p_context->p_event, NULL);
                return;
            }
        }

        delete p_context;
    }
*/

void
user::post_action_msg(string s_msgkey)
{
  get_room()->msg_post(wrap::TIMR->get_time()+" "+get_colored_bold_name()+wrap::CONF->get_elem(s_msgkey)+"<br>\n");
}

void
user::renew_timeout()
{
  timo::renew_timeout();
  double d_time_diff = wrap::TIMR->get_time_diff(t_flood_time);

  if (d_time_diff < static_cast<double>(wrap::CONF->get_int("chat.floodprotection.seconds"))
     )
  {
    if (++i_flood_messages > static_cast<double>(wrap::CONF->get_int("chat.floodprotection.messages"))
       )
    {
      room* p_room = get_room();
      if (p_room == 0)
      {
        i_flood_messages = 0;
        return;
      }

      wrap::system_message(CHATFLO+get_name()+","+p_room->get_name()+","+tool::int2string(i_flood_messages)+")");
      msg_post(wrap::CONF->colored_error_msg("chat.msgs.err.flooding"));
      if (!get_is_gag())
      {
        set_is_gag(true);
        post_action_msg("chat.msgs.floodgag");
      }
    }
  }

  else
  {
    time(&t_flood_time);
    i_flood_messages = 0;
  }
}

void
user::check_timeout( int* i_idle_timeout )
{
  double d_user_timeout = get_last_activity();
  if ( get_away() ? i_idle_timeout[1] <= d_user_timeout : i_idle_timeout[0] <= d_user_timeout )
  {
    wrap::system_message( string(TIMERTO) + "(" + get_name() + "," + tool::int2string((int)d_user_timeout) + ")");
    string s_quit = "<script language=JavaScript>top.location.href='/"
                    + wrap::CONF->get_elem("httpd.startsite")
                    + "';</script>";
    msg_post( &s_quit );
    set_online( false );

  }
  else if ( ! get_away() && i_idle_timeout[2] <= d_user_timeout )
  {
    wrap::system_message( string(TIMERAT) + "(" + get_name() + "," + tool::int2string((int)d_user_timeout) + ")");
    string s_msg = wrap::CONF->get_elem("chat.msgs.userautoawaytimeout");
    set_away( true, s_msg );
    string s_msg2 = wrap::TIMR->get_time() + " <b>" + get_colored_name()+ "</b>" + s_msg + "<br>\n";
    get_room()->msg_post( &s_msg2 );
    get_room()->reload_onlineframe();
  }
}

void
user::get_user_list( string &s_list  )
{
  if ( get_invisible() )
    return;

  s_list.append( wrap::CONF->get_elem("chat.html.onlinebefore") );

  if ( get_away() )
  {
    s_list.append("<img src=\"" + wrap::CONF->get_elem("chat.html.rangimages.location")+ "away.gif\"" )
    .append( " alt='" )
    .append( get_away_msg() )
    .append( "' title='" )
    .append( get_away_msg() )
    .append( "'" + wrap::CONF->get_elem("chat.html.rangimages.options") + "> " );
  }
  else if ( ! get_is_reg() )
  {
    string s_msgs = wrap::CONF->get_elem("chat.msgs.guest");
    s_list.append("<img src=\"" + wrap::CONF->get_elem("chat.html.rangimages.location")+ "guest.png\"" )
    .append( " alt='" )
    .append( s_msgs )
    .append( "' title='" )
    .append( s_msgs )
    .append( "'" + wrap::CONF->get_elem("chat.html.rangimages.options") + "> " );
  }
  else if ( get_status() != tool::string2int( wrap::CONF->get_elem("chat.defaultrang") ) && ! get_fake() )
  {
    string s_status = "rang" + tool::int2string( get_status() );
    string s_msgs = wrap::CONF->get_elem( "chat.msgs." + s_status );
    s_list.append("<img src=\"" + wrap::CONF->get_elem("chat.html.rangimages.location") +  tool::to_lower(s_status) + ".png\"" )
    .append( " alt='" )
    .append( s_msgs )
    .append( "' title='" )
    .append( s_msgs )
    .append( "'" + wrap::CONF->get_elem("chat.html.rangimages.options") + "> " );
  }
  else
  {
    s_list.append("<img src=\"images/blank.gif\"" + wrap::CONF->get_elem("chat.html.rangimages.options") + "> ");
  }

  s_list.append( get_colored_name() );
  s_list.append( wrap::CONF->get_elem("chat.html.onlinebehind") );
  s_list.append( "\n" );
}

void
user::check_restore_away()
{
  if ( get_away() )
  {
    get_room()->msg_post(
      new string(
        wrap::TIMR->get_time()
        + " <b>" + get_colored_name()
        + "</b> " + wrap::CONF->get_elem( "chat.msgs.unsetmodeaway" )
        + "( <font color=" + get_col2() + ">"
        + get_away_msg() + "</font>)<br>\n"
      )
    );
    set_away( false );
    get_room()->reload_onlineframe();
  }
}

void
user::reconf()
{}

void
user::dumpit()
{
  dumpable::add
  ("[user]");
  dumpable::add
  ("Name: " + get_name() +
   "; Room: " + get_room()->get_name() +
   "; Status: " + tool::int2string(get_status()));
  dumpable::add
  ("TempID: " + get_tmpid());
}

bool
user::same_rooms(user *p_user)
{
  return p_user->get_room()->get_lowercase_name()
         .compare(this->get_room()->get_lowercase_name()) == 0;
}

string
user::make_colors(string s_msg)
{
  return "<font color=\"#" + get_col1() + "\">" + s_msg + "</font>";
}

#endif
