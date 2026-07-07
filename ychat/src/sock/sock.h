/*:*
 *: File: ./src/sock/sock.h
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

#ifndef SOCK_H
#define SOCK_H

#include <vector>
#include <queue>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../reqp.h"
#include "../chat/user.h"
#include "../maps/shashmap.h"

#ifdef LOGGING
#include "../logd.h"
#endif

#include <sys/time.h>
#include <event.h>

using namespace std;

class sock
{
private:
  static vector< sock* > vec_socks;
  static int i_id_counter;
  long i_id;

protected:
#ifdef LOGGING
  logd *log_daemon; // the log daemon
#endif
  shashmap< string, unsigned, self_hash<unsigned>, equals_allocator<unsigned> > *p_ip_cache_map;

  int i_server_sock;
  int i_server_port;
  unsigned long long i_req; // total number of server requests.
  bool  b_run; // true while socket manager is running.
  reqp *p_reqp; // parses the http requests from clients.

  struct event ev_handle_server_accept;
  static void handle_server_accept(int i_fd, short event, void *p_arg);
  static void handle_client_read(int i_fd, short event, void *p_arg);
  static void handle_client_write(int i_fd, short event, void *p_arg);
  static void handle_client_stream_write(int i_fd, short event, void *p_arg);
  static void handle_stream_write(int i_fd, short event, void *p_arg);
  static void handle_stream_read(int i_fd, short event, void *p_arg);

  int set_nonblock(int i_sock);

public:
  explicit sock();
  void process_request();

  shashmap< string, unsigned, self_hash<unsigned>, equals_allocator<unsigned> > * get_ip_cache_map()
  {
    return p_ip_cache_map;
  }
  reqp *get_req_parser() const
  {
    return p_reqp;
  }
  int get_id() const
  {
    return i_id;
  }
  string dump()
  {
    return p_ip_cache_map->dump();
  }
  bool get_server_() const
  {
    return b_run;
  }
  bool get_run() const
  {
    return b_run;
  }
  bool set_run( bool b_run )
  {
    this->b_run = b_run;
  }
  void clean_ipcache();
  virtual int _make_server_socket(int i_port);
#ifdef OPENSSL
  virtual bool _main_loop_do_ssl_stuff(int& i_new_sock);
#endif

  static void init_event_handlers();
  void init_server_event_handler();
};
#endif
