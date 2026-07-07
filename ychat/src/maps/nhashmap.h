/*:*
 *: File: ./src/maps/nhashmap.h
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

#ifndef NHASHMAP_H
#define NHASHMAP_H

#include "shashmap.h"

using namespace std;

template
<
class obj_type,
class key_type_ = string,
class hash_type = size_hash<string>,
class alloc_type = compare_allocator<string>
>
struct nhashmap : public shashmap<obj_type, key_type_, hash_type, alloc_type>
{
  inline obj_type get_elem(key_type_ t_key);
};

#include "nhashmap.tmpl"
#endif
