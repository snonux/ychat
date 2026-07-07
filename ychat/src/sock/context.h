/*:*
 *: File: ./src/sock/context.h
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

#include "../incl.h"

#ifndef CONTEXT_H
#define CONTEXT_H

#include <map>
#include <string>
#include <sys/time.h>
#include <event.h>

using namespace std;

class sock;
class user;

struct context
{
  sock *p_sock;
  struct event* p_event;
  char c_buf[READSOCK+1];
  int  i_buf_len;   // accumulated bytes in c_buf (request may arrive in several reads)
  //string s_request;
  map<string,string> *p_map_params;
  int i_fd;
  string *p_response;
  user *p_user;

  context(sock *p_sock, struct event *p_event, int i_fd);
  ~context();
  void del_event();
};

#endif
