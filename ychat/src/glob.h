/*:*
 *: File: ./src/glob.h
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

#include "maps/hashmap.h"

// global variables.
#ifndef GLOB_H
#define GLOB_H

#include "config.h"

// Definition of boolean values.
#ifndef true
#define true   1
#endif
#ifndef false
#define false  0
#endif

//<<*
/* FIRST THE YCHAT ONLY OPTIONS */

#ifdef HAVE_LIBMYSQLCLIENT
#ifdef HAVE_MYSQL_MYSQL_H
#define USE_MYSQL
#define DATABASE
#define DATA_PRINT_QUERIES
#endif
#endif

#ifdef HAVE_LIBSQLITE3
#ifdef HAVE_SQLITE3_H
#define USE_SQLITE
#define DATABASE
#define DATA_PRINT_QUERIES
#endif
#endif

#define PUSHSTR 1000

/* The backlog argument defines the maximum length the queue of pending
   connections may grow to.
*/
#define BACKLOG 128*8

#ifdef HAVE_LIBSSL
#ifdef HAVE_OPENSSL_SSL_H
#define OPENSSL
#endif
#endif

#define CLI
#define CONFILE "ychat.conf"

//#define DEBUG
//#define EXPERIM

#define LOGGING
#define MAXPORT 65535
#define MAXLINES 30
#define MAXLENGTH 1024

#ifdef HAVE_LIBREADLINE
#ifdef HAVE_READLINE_READLINE_H
#define READLINE
#endif
#endif

#define POSTBUF 1024
#define READBUF 2048
#define READSOCK 16384
#define SERVMSG
//#define CTCSEGV
#define VERBOSE

using namespace std;

typedef int mod_func_t( void *v_arg );

struct container
{
  void* elem[4];
};

struct dynmod
{
  mod_func_t *the_func  ;
  void     *the_module;
};

typedef enum method_
{
  METH_RETSTRING
} method;

// Define external executables:
#define GMAKE "/usr/local/bin/gmake "

#endif
