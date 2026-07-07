/*:*
 *: File: ./src/data/data.h
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
#ifdef DATABASE

#ifndef DATA_H
#define DATA_H

#include "data_base.h"
#ifdef USE_SQLITE
#include <sqlite3.h>
#else
#include <mysql/mysql.h>
#endif

using namespace std;

// Named ychatdb, not "data": a class literally named "data" collides with
// std::data() (C++17) under "using namespace std" - GCC treats it as
// "reference to 'data' is ambiguous". Same class of fix as the
// function->mod_func_t rename in glob.h and attributes::set_attr_flag in
// ../ycurses.
class ychatdb : public data_base // db implementation used by wrap::DATA
{
private:
#ifdef USE_SQLITE
  // Parameter binding (sqlite3_bind_text) does its own escaping, so unlike
  // the MySQL path there is no separate secure_query()/parse_result() step.
  hashmap<string> select_query( string s_query, string s_nick, vector<string>& vec_elements );
  void insert_query( string s_query, map<string,string> map_insert );
#else
  MYSQL_RES* select_query( string s_query, string s_where_rule, vector<string>& vec_elements );
  hashmap<string> parse_result( MYSQL_RES* p_result, vector<string>& vec_elements );
  void insert_query( string s_query, map<string,string> map_insert );
  string secure_query( string s_mysql_query );
#endif
public:
  ychatdb( );
  ~ychatdb( );

  hashmap<string> select_user_data( string s_user, string s_query );
  void insert_user_data( string s_user, string s_query, map<string,string> insert_map );
  void update_user_data( string s_user, string s_query, hashmap<string> update_map );
};

#endif
#endif
