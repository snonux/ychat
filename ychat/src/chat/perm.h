/*:*
 *: File: ./src/chat/perm.h
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.3-CURRENT
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
#include "../maps/shashmap.h"

#ifndef PERM_H
#define PERM_H

using namespace std;

template<class type_>
struct valwrap
{
  type_ val;
  valwrap(type_ i)
  {
    val = i;
  }

  type_ get_val()
  {
    return val;
  }
};

class perm
{
private:
  shashmap< valwrap<int>* >* p_command_status;
  shashmap< valwrap<bool>* >* p_command_disabled;

  void set_standard_command_permissions();

public:
  perm( );
  ~perm( );

  virtual int  get_command_status( string s_command );
  virtual bool get_command_disabled( string s_command );
  virtual void set_command_status( string s_command, int i_int );
  virtual void set_command_disabled( string s_command, bool b_bool );
};

#endif
