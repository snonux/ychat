/*:*
 *: File: ./src/modl.h
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

#include "incl.h"

#ifndef MODL_H
#define MODL_H

#include "maps/shashmap.h"

using namespace std;

class modl : public shashmap<dynmod*>
{
private:
  static  void dlclose_( dynmod* mod   );
  dynmod* cache_module ( string s_name, bool b_print_sys_msg );
  void preload_modules( string s_path );

public:
  modl();
  ~modl();

  dynmod* get_module( string s_name );
  dynmod* get_module( string s_name, string s_user );

  vector<string>* get_mod_vector()
  {
    vector<string>* p_ret = get_key_vector();
    return p_ret;
  }

  void unload_modules();
  void reload_modules();
};

#endif
