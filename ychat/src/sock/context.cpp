/*:*
 *: File: ./src/sock/context.cpp
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

#ifndef CONTEXT_CPP
#define CONTEXT_CPP

#include "context.h"

using namespace std;

context::context(sock *p_sock, struct event *p_event, int i_fd)
{
  this->p_sock = p_sock;
  this->p_event = p_event;
  this->i_fd = i_fd;
  this->i_buf_len = 0;
  this->c_buf[0] = '\0';
  this->p_map_params = NULL;
  this->p_response = NULL;
  this->p_user = NULL;
}

context::~context()
{
  shutdown(i_fd, 2);
  close(i_fd);

  if (this->p_event)
    delete this->p_event;

  if (this->p_map_params)
    delete this->p_map_params;

  if (this->p_response)
    delete this->p_response;
}

void
context::del_event()
{
  if (this->p_event)
    delete this->p_event;
  this->p_event = NULL;
}
#endif
