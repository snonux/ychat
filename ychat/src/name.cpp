/*:*
 *: File: ./src/name.cpp
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

#ifndef NAME_CPP
#define NAME_CPP

#include "name.h"
#include "tool/tool.h"

using namespace std;

name::name()
{}

name::name( string s_name )
{
  set_name( s_name );
}

name::~name()
{}

string
name::get_name()
{
  string s_ret;
  s_ret = s_name;
  return s_ret;
}

string
name::get_lowercase_name()
{
  return tool::to_lower( get_name() );
}

void
name::set_name( string s_name )
{
  this->s_name = s_name;
}

#endif
