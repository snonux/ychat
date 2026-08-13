/*:*
 *: File: ./src/chat/sess.cpp
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

#ifndef SESS_CPP
#define SESS_CPP

#include "sess.h"

sess::sess( string s_tmpid )
{
  this->p_user = NULL;
  this->s_tmpid = s_tmpid;
}

sess::~sess()
{}

string
sess::get_tmpid()
{
  return s_tmpid;
}

void
sess::set_tmpid(string s_tmpid)
{
  this->s_tmpid = s_tmpid;
}

string
sess::get_name()
{
  if ( p_user = NULL )
    return "";

  return p_user->get_name();
}

void
sess::set_name(string s_name)
{
  if ( p_user != NULL )
    p_user->set_name(s_name);
}

user*
sess::get_user()
{
  return p_user;
}

void
sess::set_user(user* p_user)
{
  this->p_user = p_user;
  //p_user->set_sess(this);
}

/*
void
sess::invalidate()
{
  set_user(NULL);
  set_tmpid("!invalidated");
  wrap::system_message(SESSIOI);
}
*/

#endif
