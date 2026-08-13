/*:*
 *: File: ./src/maps/shashmap.h
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

#ifndef SHASHMAP_H
#define SHASHMAP_H

#include "hashmap.h"

#include "../monitor/dump.h"

using namespace std;

template
<
class obj_type,
class key_type_ = string,
class hash_type = size_hash<string>,
class alloc_type = compare_allocator<string>
>
class shashmap : protected hashmap<obj_type, key_type_, hash_type, alloc_type>,
      public dumpable
{
private:

protected:
  virtual void dumpit();

public:
  explicit shashmap();
  ~shashmap();
  virtual inline void set_elem(obj_type t_obj, key_type_ t_key);
  virtual inline obj_type get_set_elem(obj_type t_obj, key_type_ t_key);
  virtual inline obj_type get_or_callback_set
  (obj_type (*func)(void*), void* p_void, key_type_ t_key);
  virtual inline void add_elem(obj_type t_obj, key_type_ t_key);
  virtual inline void add_elem_insecure(obj_type t_obj, key_type_ t_key);
  virtual inline obj_type get_elem(key_type_ t_key);
  virtual inline void del_elem(key_type_ t_key);
  virtual inline void del_elem_insecure(key_type_ t_key);
  virtual inline void clear();
  virtual inline int size();
  virtual inline bool exists(key_type_ t_key);
  virtual inline vector<key_type_>* get_key_vector();
  virtual inline void run_func( void (*func)(obj_type) );
  virtual inline void run_func( void (*func)(obj_type, void*), void* v_arg );

};

#include "shashmap.tmpl"
#endif
