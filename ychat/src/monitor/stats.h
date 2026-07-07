/*:*
 *: File: ./src/monitor/stats.h
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

#include "../incl.h"

#ifndef STATS_H
#define STATS_H

#include "../tool/tool.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

class stats
{
private:
  // Specifies the max. amount of elements in vec_rusage_history;
  int i_rusage_vec_size;
  // History of the last i_rusage_vec_size rusage values.
  vector< map<string,long> > vec_rusage_history;

  int i_num_rooms; //<<

  void set_rusage_vec_size( int i_rusage_vec_size );

public:
  stats( );
  ~stats( );

  //<<*
  int get_num_rooms();
  void increment_num_rooms();
  void decrement_num_rooms();
  //*>>
  void update_rusage_history();
  string get_rusage_history( string s_type, string s_seperator );
  long get_ru_maxrss();
};

#endif
