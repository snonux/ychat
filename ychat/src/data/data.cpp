/*:*
 *: File: ./src/data/data.cpp
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
#include "data.h"

#ifdef DATABASE
#ifndef DATA_CPP
#define DATA_CPP

using namespace std;

#ifdef USE_SQLITE

ychatdb::ychatdb()
{}

ychatdb::~ychatdb()
{}

hashmap<string>
ychatdb::select_user_data( string s_user, string s_query)
{
  vector<string> vec_elements;
  return select_query( s_query, s_user, vec_elements );
}

hashmap<string>
ychatdb::select_query( string s_query, string s_nick, vector<string>& vec_elements )
{
  hashmap<string> map_ret;
  con* p_con = get_con();

  vec_elements = map_queries[s_query];
  if ( vec_elements.size() == 0 )
  {
    push_con( p_con );
    return map_ret;
  }

  vector<string>::iterator iter = vec_elements.begin();
  string s_table = *iter;
  iter++;

  string s_sql = "SELECT ";
  for ( vector<string>::iterator it = iter; it != vec_elements.end(); )
  {
    s_sql.append( *it );
    if ( ++it != vec_elements.end() )
      s_sql.append( ", " );
  }
  s_sql.append( " FROM " + s_table + " WHERE nick = ?" );

  print_query( SQLITEQU + s_sql );

  sqlite3_stmt* p_stmt = NULL;
  if ( sqlite3_prepare_v2( p_con->p_sqlite, s_sql.c_str(), -1, &p_stmt, NULL ) == SQLITE_OK )
  {
    sqlite3_bind_text( p_stmt, 1, s_nick.c_str(), -1, SQLITE_TRANSIENT );

    if ( sqlite3_step( p_stmt ) == SQLITE_ROW )
    {
      int i_cols = sqlite3_column_count( p_stmt );
      vector<string>::iterator col_iter = iter;
      for ( int i = 0; i < i_cols && col_iter != vec_elements.end(); i++, col_iter++ )
      {
        const unsigned char* c_val = sqlite3_column_text( p_stmt, i );
        map_ret[*col_iter] = c_val ? string( (const char*) c_val ) : "";
      }
    }
  }
  else
  {
    wrap::system_message( SQLITEE2 + string( sqlite3_errmsg( p_con->p_sqlite ) ) );
  }

  sqlite3_finalize( p_stmt );
  push_con( p_con );
  return map_ret;
}

void
ychatdb::insert_user_data( string s_user, string s_query, map<string,string> insert_map )
{
  insert_query( s_query, insert_map );
}

void
ychatdb::insert_query( string s_query, map<string,string> map_insert )
{
  vector<string> vec_elements = map_queries[s_query];
  if ( vec_elements.size() == 0 )
    return;

  vector<string>::iterator iter = vec_elements.begin();
  string s_table = *iter;
  iter++;

  string s_cols, s_placeholders;
  for ( vector<string>::iterator it = iter; it != vec_elements.end(); )
  {
    s_cols.append( *it );
    s_placeholders.append( "?" );
    if ( ++it != vec_elements.end() )
    {
      s_cols.append( ", " );
      s_placeholders.append( ", " );
    }
  }

  string s_sql = "INSERT INTO " + s_table + " (" + s_cols + ") VALUES (" + s_placeholders + ")";
  print_query( SQLITEQU + s_sql );

  con* p_con = get_con();
  sqlite3_stmt* p_stmt = NULL;

  if ( sqlite3_prepare_v2( p_con->p_sqlite, s_sql.c_str(), -1, &p_stmt, NULL ) == SQLITE_OK )
  {
    int i = 1;
    for ( iter = vec_elements.begin() + 1; iter != vec_elements.end(); iter++, i++ )
      sqlite3_bind_text( p_stmt, i, map_insert[*iter].c_str(), -1, SQLITE_TRANSIENT );

    if ( sqlite3_step( p_stmt ) != SQLITE_DONE )
      wrap::system_message( SQLITEE2 + string( sqlite3_errmsg( p_con->p_sqlite ) ) );
  }
  else
  {
    wrap::system_message( SQLITEE2 + string( sqlite3_errmsg( p_con->p_sqlite ) ) );
  }

  sqlite3_finalize( p_stmt );
  push_con( p_con );
}

void
ychatdb::update_user_data( string s_user, string s_query, hashmap<string> update_map )
{
  vector<string> vec_elements = map_queries[s_query];
  if ( vec_elements.size() == 0 )
    return;

  vector<string>::iterator iter = vec_elements.begin();
  string s_table = *iter;
  iter++;

  string s_sql = "UPDATE " + s_table + " SET ";
  vector<string> vec_bind_cols;
  bool b_flag = 0;

  for ( ; iter != vec_elements.end(); iter++ )
  {
    if ( update_map[*iter] == "" ) // Dont update data if it has not been changed / if its empty!
      continue;

    if ( b_flag )
      s_sql.append( ", " );

    s_sql.append( *iter + "=?" );
    vec_bind_cols.push_back( *iter );
    b_flag = 1;
  }

  if ( !b_flag )
    return;

  s_sql.append( " WHERE nick=?" );
  print_query( SQLITEQU + s_sql );

  con* p_con = get_con();
  sqlite3_stmt* p_stmt = NULL;

  if ( sqlite3_prepare_v2( p_con->p_sqlite, s_sql.c_str(), -1, &p_stmt, NULL ) == SQLITE_OK )
  {
    int i = 1;
    for ( vector<string>::iterator col_iter = vec_bind_cols.begin(); col_iter != vec_bind_cols.end(); col_iter++, i++ )
      sqlite3_bind_text( p_stmt, i, update_map[*col_iter].c_str(), -1, SQLITE_TRANSIENT );

    string s_lower_user = tool::to_lower(s_user);
    sqlite3_bind_text( p_stmt, i, s_lower_user.c_str(), -1, SQLITE_TRANSIENT );

    if ( sqlite3_step( p_stmt ) != SQLITE_DONE )
      wrap::system_message( SQLITEE2 + string( sqlite3_errmsg( p_con->p_sqlite ) ) );
  }
  else
  {
    wrap::system_message( SQLITEE2 + string( sqlite3_errmsg( p_con->p_sqlite ) ) );
  }

  sqlite3_finalize( p_stmt );
  push_con( p_con );
}

#else

ychatdb::ychatdb()
{}

ychatdb::~ychatdb()
{}

hashmap<string>
ychatdb::select_user_data( string s_user, string s_query)
{
  string s_where_rule = " WHERE nick = \"" + s_user + "\"";
  vector<string> vec_elements;
  MYSQL_RES* p_result = select_query( s_query, s_where_rule, vec_elements );
  return parse_result( p_result, vec_elements );
}

MYSQL_RES*
ychatdb::select_query( string s_query, string s_where_rule, vector<string>& vec_elements )
{
  con* p_con = get_con();

  vec_elements = map_queries[s_query];
  string s_mysql_query = "SELECT ";
  vector<string>::iterator iter = vec_elements.begin();

  string s_table = *iter;
  iter++;

  while ( iter != vec_elements.end() )
  {
    s_mysql_query.append( secure_query(*iter) );
    if ( ++iter != vec_elements.end() )
      s_mysql_query.append( ", " );
  }

  s_mysql_query.append(" FROM " + s_table + s_where_rule );
  print_query( MYSQLQU + s_mysql_query );

  MYSQL_RES* p_result = NULL;

  if ( 0 == mysql_query( p_con->p_mysql, (const char*)s_mysql_query.c_str() ) )
  {
    p_result = mysql_store_result( p_con->p_mysql );
    push_con( p_con );
  }
  else
  {
    wrap::system_message( MYSQLQU + string( mysql_error(p_con->p_mysql) ) );
    if (p_con != NULL)
      delete p_con;
  }

  return p_result;
}

hashmap<string>
ychatdb::parse_result( MYSQL_RES* p_result, vector<string>& vec_elements )
{
  hashmap<string> map_ret;
  if ( p_result != NULL )
  {
    MYSQL_ROW row;
    vector<string>::iterator vec_iter = vec_elements.begin();
    vec_iter++;

    while ( (row = mysql_fetch_row(p_result)) )
      for ( int i=0; i < mysql_num_fields(p_result); i++, vec_iter++ )
        map_ret[*vec_iter] = string(row[i]);

    mysql_free_result( p_result );
  }
  return map_ret;
}

void
ychatdb::insert_user_data( string s_user, string s_query, map<string,string> insert_map )
{
  insert_query( s_query, insert_map );
}

void
ychatdb::insert_query( string s_query, map<string,string> map_insert )
{
  vector<string> vec_elements = map_queries[s_query];
  vector<string>::iterator iter = vec_elements.begin();

  string s_table = *iter;
  iter++;
  string s_mysql_query = "INSERT INTO " + s_table + " (";

  while ( iter != vec_elements.end() )
  {
    s_mysql_query.append( *iter );

    if ( ++iter != vec_elements.end() )
      s_mysql_query.append( ", " );
    else
      s_mysql_query.append( ") VALUES(" );
  }

  iter = vec_elements.begin();
  iter++;

  while ( iter != vec_elements.end() )
  {
    s_mysql_query.append( "\"" + secure_query(map_insert[*iter]) + "\"" );
    if ( ++iter != vec_elements.end() )
      s_mysql_query.append( ", " );
    else
      s_mysql_query.append( ")" );
  }

  print_query( MYSQLQU + s_mysql_query );

  con* p_con = get_con();

  if ( 0 != mysql_query( p_con->p_mysql, (const char*)s_mysql_query.c_str() ) )
    wrap::system_message( MYSQLQU + string( mysql_error(p_con->p_mysql) ) );

  push_con( p_con );

  return;
}

void
ychatdb::update_user_data( string s_user, string s_query, hashmap<string> update_map )
{
  vector<string> vec_elements = map_queries[s_query];

  if ( vec_elements.size() == 0 )
    return;

  vector<string>::iterator iter = vec_elements.begin();
  vector<string>::iterator iter_second = vec_elements.begin();
  iter_second++;

  string s_table = *iter;
  iter++;
  string s_mysql_query = "UPDATE " + s_table + " SET ";
  bool b_flag = 0;

  while ( iter != vec_elements.end() )
  {
    if ( update_map[*iter] == "" ) // Dont update data if it has not been changed / if its empty!
    {
      iter++;
      continue;
    }

    if ( iter != iter_second && b_flag )
      s_mysql_query.append( ", " );

    s_mysql_query.append( *iter + "=\"" + secure_query(update_map[*iter]) + "\"" );
    b_flag = 1;
    iter++;
  }

  if ( b_flag )
  {
    s_mysql_query.append( " WHERE nick=\"" + tool::to_lower(s_user) + "\"" );

    con* p_con = get_con();
    print_query( MYSQLQU + s_mysql_query );

    if ( 0 != mysql_query( p_con->p_mysql, (const char*)s_mysql_query.c_str() ) )
      wrap::system_message( MYSQLQU + string( mysql_error(p_con->p_mysql) ) );

    push_con( p_con );
  }
}

string
ychatdb::secure_query( string s_mysql_query )
{
  // Prevent from MySQL injection attacks (escaping " and \)
  unsigned i_pos = s_mysql_query.find("\\");

  while ( i_pos != string::npos )
  {
    s_mysql_query.replace( i_pos, 1, "/" );
    i_pos = s_mysql_query.find("\\");
  }

  i_pos = s_mysql_query.find("\"");

  while ( i_pos != string::npos )
  {
    s_mysql_query.replace( i_pos, 1, "'" );
    i_pos = s_mysql_query.find("\"");
  }

  return s_mysql_query;
}

#endif

#endif
#endif
