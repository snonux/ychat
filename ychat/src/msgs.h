/*:*
 *: File: ./src/msgs.h
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

#ifndef MSGS_H
#define MSGS_H

#include "build.h"

// alphabetical ordered:
#define ACCPERR "Sock: Accept error "
#define BINDERR "Sock: Bind error "
#define HTTPERR "Sock: Invalid HTTP request "
#define NONBLER "Sock: Could not set server socket nonblocking"
#define NONBCER "Sock: Could not set client socket nonblocking"
#define CHATREP "Chat: Using replacement strings"
#define CHATDOP "Chat: Default operator login "
#define CHATFLO "Chat: Flooding ("
#define CFILEOK "Parsing config file"
#define CFILEFA "Failed opening config file!"
#define CONTACT "Contact: https://codeberg.org/snonux/ychat, Mail@yChat.org, ICQ: 11655527"
#define CLRHTML "HTML: Cleared the document cache "
#define CLIWELC "Command Line Interface (type help for a list of all commands)"
#define CLIPRMO ">> "
#define CLIPRMI "<< "
#define CLIHELP "Unknown command (use help)"
#define CLIMSQL "Spawing system mysql client (enter exit to return)"
#define CLISHEL "Spawing system shell (enter exit to return)"
#define DATAADD "Data: Adding used connection into the queue"
#define DATADIS "Data: Closing all connections"
#define DATADI2 "Data: Closing idle connection ("
#define DATAQUE "Data: "
#define DATAGET "Data: Using database connection queue "
#define DATAIN0 "Data: Initializing maxcon to "
#define DATAIN1 "Data: Initializing mincon to "
#define DATAMAX "Data: Max database connections reached "
#define DATAMA0 "Data: Max database connections "
#define DATANEW "Data: Creating new database connection "
#define DESCRIP "Copyright (C) 2003 Paul C. Buetow, Volker Richter"
#define DESCRI2 "Copyright (C) 2004, 2005 Paul C. Buetow"
#define DESCRI3 "Copyright (C) 2005 EXA Digital Solutions GbR"
#define DONEMSG "done"
#define DOWNMSG "Shutting down "
#define EVENTSO "Initializing sock events"
#define GARBAGE "Garbage: Initializing collector "
#define GARBACT "Garbage collector activated "
#define GAROFFNE "Garbage: No garbage to remove available "
#define GARROOM "Garbage: Added room "
#define GARUSER "Garbage: Added user "
#define GARUSE2 "Garbage: Recycle user "
#define LISTERR "Sock: Listen error "
#define LOGGINI "Logging: Init on "
#define LOGERR1 "Logging: Could not open logfile "
#define LOGERR2 "Logging: No filename specified "
#define LOGINER "Chat: Login failed (password), nick: "
#define LOGINE0 "Chat: Login failed (empty nick)"
#define LOGINE1 "Chat: Login failed (alpha nick), nick: "
#define LOGINE2 "Chat: Login failed (nick length), nick: "
#define LOGINE3 "Chat: Login failed (room length), nick/room: "
#define LOGINE4 "Chat: Login failed (guests disabled), nick: "
#define LOGINE5 "Chat: Login failed (not enough threads), nick: "
#define MODULEC "Modl: Caching "
#define MODULER "Modl: Requesting "
#define MODUNLO "Modl: Unloading all modules "
#define MODRELO "Modl: Reloading all modules "

#ifdef DATABASE
#define MYSQLQU "MySQL: "
#define MYSQLQ2 "MySQL: Adding query "
#define MYSQLE1 "MySQL: Error running mysql_init "
#define SQLITEQU "SQLite: "
#define SQLITEE1 "SQLite: Error opening database file "
#define SQLITEE2 "SQLite: Error running query: "
#endif

#define SIGNALR "Signal: Received "
#define NEWREQU "Sock: New request "
#define NEWROOM "Chat: New room "
#define NEWUSER "Chat: New user "
#define OFFFOUND "HTML: File not found "
#define PERMSTD "Reading standard command exec permissions"
#define READERR "Sock: Read error"
#define REMROOM "Garbage: Removing room "
#define REMUSER "Garbage: Removing user "
#define REQUEST "Reqp: Request string "
#define SEPERAT "-----------------------"
#define SESSIOC "Session: Create "
#define SESSIOD "Session: Destroy "
#define SESSIOI "Session: Invalidated a session "
#define SESSION "Session: Count "
#define SESSDMP "Session: Dump of session"
#define SESSERR "Session: Could not find session "
#define SESSEXI "Session: New TempID already exists, recalc."
#define SHELLER "Shell: Could not execute command"
#define SHELLEX "Shell: Executing the following command:"
#define SIGSIGV "Signal: SIGV received!"
#define STATUPR "Stats: Updated rusage history"
#define STATRSS "Stats: Max resident set size "
#define REUROOM "Garbage: Reusing room object "
#define SOCKCLN "Sock: Initializing a client socket at "
#define SOCKCON "Sock: Connecting to "
#define SOCKCRT "Sock: Created socket on "
#define SOCKERR "Sock: Can't create socket, trying next port "
#define SOCKER1 "Sock: Can't create socket, aborting"
#define SOCKER2 "Sock: Unknown hostname "
#define SOCKER4 "Sock: Incomplete HTTP request"
#define SOCKER5 "Sock: Could not set socket nonblocking"
#define SOCKRDY "Sock: Server socket is ready "
#define SOCKCAC "Sock: Caching IP "
#define SOCKCA2 "Sock: Cleaning IP cache ("
#define SOCKUNS "Sock: Starting unsecure transport [HTTP]"
#ifdef OPENSSL
#define SSLERR1 "SSL: Can't create socket"
#define SSLERR2 "SSL: Private key does not match cert. file"
#define SSLERR3 "SSL: Can't create new SSL context"
#define SSLERR4 "SSL: Can't create new SSL socket via accept"
#define SOCKSEC "SSL: Starting secure transport [HTTPS]"
#endif
#define TECACHE "HTML: Caching document "
#define TECACHN "HTML: Not caching document (caching disabled) "
#define THRDSTR "Thread: Running"
#define TIMERAT "Timer: User autoaway timeout "
#define TIMERIN "Timer: Initializing "
#define TIMEROF "Timer: Setting offset to "
#define TIMERTH "Timer: Starting timer thread "
#define TIMERTO "Timer: User logout timeout "
#define TIMERUP "Timer: System uptime "
#define XMLREAD "XML: Reading "
#define XMLERR "XML Error: "
#define XMLER1 "XML Error: Unable to load file "
#define YCUSAGE "Usage: ./ychat {h|v}|{o confkey confvalue}\n"

#define HEADER1 "HTTP/1.1 200 OK\r\n"
#define HEADER2 "Server: yChat/" VERSION "-" BRANCH "\r\n"
#define HEADER3 "Cache-Control: no-cache\r\n"
//#define HEADER4 ""
#define HEADER4 "Pragma: no-cache\r\nConnection: close\r\n"
//#define HEADER4 "Pragma: no-cache\r\nDate: Sat, 09 Sep 2006 03:22:06 GMT\r\nConnection: close\r\n"
#define HEADER5 "Transfer-Encoding: chunked\r\n"
#define HEADER6 "Connection: keep-alive\r\n"
#define HEADER7 "Content-Length: ";
#define HEADER8 "Content-Type: ";
#define HEADER8b "; charset=ISO-8859-1\r\n";
#define HEADER9 "";
//#define HEADER9 "Accept-Ranges: bytes\r\n";
//#define HEADER9 "Keep-Alive: timeout=15, max=99\r\nLast-Modified: Mon, 19 Dec 2005 23:00:02 GMT\r\nAccept-Ranges: bytes\r\nDate: Tue, 20 Dec 2005 03:30:10 GMT\r\n";
//#define MEMBERE "Memb: No such member "

#endif
