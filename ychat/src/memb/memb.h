/*:*
 *: File: ./src/memb/memb.h
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

#ifndef MEMB_H
#define MEMB_H

#include "../incl.h"
#include "../wrap.h"
#include "../maps/hashmap.h"
#include "tupel.h"

using namespace std;

template<class obj_type>
class memb
{
private:
  hashmap< mutexed_tupel<obj_type>* > map_elems;

public:
  memb()
  {}
  memb(vector<string> &vec_fields);
  void initialize(vector<string> &vec_fields);
  inline obj_type get_elem(string s_key);
  inline void set_elem(obj_type t_obj, string s_key);
};

struct memb_string : protected memb<string>
{
  memb_string()
  {}
  memb_string(vector<string> &vec_fields) : memb<string>(vec_fields)
  {}
  void init_strings(vector<string> vec_fields)
  {
    initialize(vec_fields);
  }
  inline string get_string(string s_key);
  inline void set_string(string s_obj, string s_key);
};

struct memb_int : protected memb<int>
{
  memb_int()
  {}
  void init_ints(vector<string> vec_fields)
  {
    initialize(vec_fields);
  }
  inline int get_int(string s_key);
  inline void set_int(int i_obj, string s_key);
};

struct memb_bool : protected memb<bool>
{
  memb_bool()
  {}
  void init_bools(vector<string> vec_fields)
  {
    initialize(vec_fields);
  }
  inline bool get_bool(string s_key);
  inline void set_bool(bool b_obj, string s_key);
};

class memb_base : public memb_string, public memb_int, public memb_bool
  {}
;

#include "memb.tmpl"
#endif
