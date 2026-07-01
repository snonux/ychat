/*:*
 *: File: ./src/data/con.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.0-CURRENT
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
#include "con.h"

using namespace std;

#ifdef DATABASE
#ifndef CON_CPP
#define CON_CPP

#ifdef USE_SQLITE

// Schema for the single "user" table this legacy data layer ever queries
// (see etc/ychat.conf's chat.database.mysql* entries for the column list
// the queries are built from: nick password color1 color2 email
// registerdate status). MySQL deployments are expected to have this table
// created out-of-band; SQLite has no such out-of-band step, so create it
// here if missing.
#define SQLITE_USER_TABLE_DDL \
  "CREATE TABLE IF NOT EXISTS user (" \
  "nick TEXT PRIMARY KEY, password TEXT, color1 TEXT, color2 TEXT, " \
  "email TEXT, registerdate TEXT, status TEXT)"

con::con()
{
  // chat.database.dbname doubles as the SQLite file path in this mode
  // (there is no separate server/user/password/port for an embedded db).
  string s_path = wrap::CONF->get_elem("chat.database.dbname");

  while ( sqlite3_open( s_path.c_str(), &p_sqlite ) != SQLITE_OK )
  {
    wrap::system_message( SQLITEE1 + s_path + ": " + string( sqlite3_errmsg(p_sqlite) ) );
    sqlite3_close( p_sqlite );
    usleep( 30000000 );
  }

  // Multiple pooled connections open the same file concurrently: WAL lets
  // readers and a writer coexist, and the busy timeout makes a writer wait
  // for a lock instead of failing immediately with SQLITE_BUSY (this data
  // layer has no query-retry logic of its own).
  sqlite3_busy_timeout( p_sqlite, 5000 );
  sqlite3_exec( p_sqlite, "PRAGMA journal_mode=WAL", NULL, NULL, NULL );
  sqlite3_exec( p_sqlite, "PRAGMA foreign_keys=ON", NULL, NULL, NULL );

  char* c_err = NULL;
  if ( sqlite3_exec( p_sqlite, SQLITE_USER_TABLE_DDL, NULL, NULL, &c_err ) != SQLITE_OK )
  {
    wrap::system_message( SQLITEE2 + string( c_err ? c_err : "unknown error" ) );
    sqlite3_free( c_err );
  }
}

con::~con()
{
  if ( p_sqlite )
    sqlite3_close( p_sqlite );
}

#else

con::con()
{
  p_mysql = mysql_init(NULL);

  while ( !p_mysql )
  {
    wrap::system_message( MYSQLE1 );
    usleep( 30000000 );
    mysql_init(p_mysql);
  }

  while ( mysql_real_connect(
            p_mysql,
            (const char*)wrap::CONF->get_elem("chat.database.serverhost").c_str(),
            (const char*)wrap::CONF->get_elem("chat.database.user").c_str(),
            (const char*)wrap::CONF->get_elem("chat.database.password").c_str(),
            (const char*)wrap::CONF->get_elem("chat.database.dbname").c_str(),
            tool::string2int(wrap::CONF->get_elem("chat.database.port")),
            NULL, 0 ) == NULL )
  {
    wrap::system_message( MYSQLQU + string( mysql_error(p_mysql) ) );
    usleep( 30000000 );
  }
}

con::~con()
{
  if ( p_mysql )
  {
    if (mysql_ping( p_mysql ) != 0)
      mysql_close( p_mysql );
  }
}

#endif

#endif
#endif
