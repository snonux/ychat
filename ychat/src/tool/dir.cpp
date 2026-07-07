/*:*
 *: File: ./src/tool/dir.cpp
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

#ifndef DIR_CPP
#define DIR_CPP

#include "dir.h"

using namespace std;

dir::dir()
{
  b_open = false;
}

dir::~dir()
{
  vec_dir.clear();
  close_dir();
}

bool
dir::open_dir( char *c_dir )
{
  string s_dir( c_dir );
  return open_dir( s_dir  );
}

bool
dir::open_dir( string &s_dir )
{
  if ( b_open )
    return false;

  p_d = opendir( s_dir.c_str() );

  if ( p_d == NULL )
    return false; // Could not open dir.

  b_open = true;

  return true; // Could open dir with success.
}

void
dir::close_dir()
{
  if ( b_open && p_d != NULL )
  {
    closedir( p_d );
    b_open = false;
  }
}

void
dir::read_dir()
{
  if ( p_d != NULL )
    while ( p_ep = readdir( p_d ) )
      vec_dir.push_back( string( p_ep->d_name ) );
}

vector<string>
dir::get_dir_vec()
{
  return vec_dir;
}

#endif
