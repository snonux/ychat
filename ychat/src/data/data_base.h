/*:*
 *: File: ./src/data/data_base.h
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
#ifdef DATABASE

#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <vector>
#include <list>
#include "con.h"

using namespace std;

class data_base : protected list<con*>
{
private:
  int i_max_con;

protected:
  hashmap< vector<string> > map_queries;
  void print_query( string s_query );
#ifdef DATA_PRINT_QUERIES

  virtual void print_query_( string s_query );
#endif

  con* get_con();
  void push_con( con* p_con );

public:
  data_base();
  ~data_base();

  void init_connections();
  virtual hashmap<string> select_user_data( string s_user, string s_query );
  virtual void insert_user_data( string s_user, string s_query, hashmap<string> insert_map );
  virtual void update_user_data( string s_user, string s_query, hashmap<string> update_map );
  void disconnect_all_connections();
  void check_data_con_timeout();
};

#endif
#endif
