/*:*
 *: File: ./src/chat/sman.h
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

#ifndef SMAN_H
#define SMAN_H

#include <cstdlib>

#include "sess.h"
#include "../maps/shashmap.h"
#include "../monitor/dump.h"

using namespace std;

class sman : public shashmap<sess*>
{
private:
  string generate_id( int i_len );
  int i_session_count;
  int i_continous_session_count;


public:
  sman();
  ~sman();
  sess *get_session( string s_tmpid );
  int get_session_count( );
  sess *create_session( );
  void destroy_session( string s_tmpid );

  //virtual void dump();
};


#endif

