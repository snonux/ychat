/*:*
 *: File: ./src/wrap.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.5-CURRENT
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

#ifndef WRAP_CPP
#define WRAP_CPP

#include "wrap.h"

using namespace std;

//<<*
chat* wrap::CHAT = NULL;
#ifdef DATABASE
ychatdb* wrap::DATA = NULL;
#endif
gcol* wrap::GCOL = NULL;
sman* wrap::SMAN = NULL;
modl* wrap::MODL = NULL;
//*>>

conf* wrap::CONF = NULL;
html* wrap::HTML = NULL;
#ifdef LOGGING
logd* wrap::LOGD = NULL;
#endif
sock* wrap::SOCK = NULL;
stats* wrap::STAT = NULL;
timr* wrap::TIMR = NULL;
dynamic_wrap* wrap::WRAP = NULL;

void
wrap::system_message(char* c_message, int i_code)
{
  wrap::system_message(string(c_message)+" ("+tool::int2string(i_code)+")");
}

void
wrap::system_message(const char* c_message, int i_code)
{
  wrap::system_message(string(c_message)+" ("+tool::int2string(i_code)+")");
}

void
wrap::system_message(char* c_message)
{
  wrap::system_message(string(c_message));
}

void
wrap::system_message(const char* c_message)
{
  wrap::system_message(string(c_message));
}

void
wrap::system_message(string* p_message, int i_code)
{
  wrap::system_message(*p_message+" ("+tool::int2string(i_code)+")");
}

void
wrap::system_message(string* p_message)
{
  wrap::system_message(*p_message);
}

void
wrap::system_message(string s_message, int i_code)
{
  wrap::system_message(s_message+" ("+tool::int2string(i_code)+")");
}

void
wrap::system_message( string s_message )
{
#ifdef SERVMSG
  cout << s_message << endl;
#endif

#ifdef LOGGING

  LOGD->log_simple_line( s_message + "\n" );
#endif
}

void
wrap::init_wrapper(map<string,string>* p_main_loop_params)
{
  // Init the dynamic wrapper (is needed to pass all wrapped objects through a single pointer).
  WRAP = new dynamic_wrap;

  // Init the config manager.
  WRAP->CONF = CONF = new conf( CONFILE, p_main_loop_params );
  delete p_main_loop_params,

  // Init the statistic manager.
  WRAP->STAT = STAT = new stats;

  // Init the html-template manager.
  WRAP->HTML = HTML = new html;

#ifdef LOGGING
  // Init the system message logd
  WRAP->LOGD = LOGD = new logd( CONF->get_elem("httpd.logging.systemfile"),
                                CONF->get_elem("httpd.logging.systemlines") );
#endif

  //<<*
  // Init the session manager.
  WRAP->SMAN = SMAN = new sman;
  //*>>
  // Init the socket manager.
  int i_port = tool::string2int( wrap::CONF->get_elem( "httpd.serverport" ) );

#ifndef OPENSSL
  WRAP->SOCK = SOCK = new sock;
#else

  WRAP->SOCK = SOCK = new sslsock;
#endif

  // create the server socket and set it up to accept connections.
  if (SOCK->_make_server_socket(i_port) <= 0)
  {
    system_message(SOCKER1);
    exit(-1);
  }

  //<<*
  // Init the chat manager.
  WRAP->CHAT = CHAT = new chat;
  //*>>

  // Init the system timer.
  WRAP->TIMR = TIMR = new timr;

  //<<*
  // Init the module-loader manager.
  WRAP->MODL = MODL = new modl;

  // Init the garbage collector
  WRAP->GCOL = GCOL = new gcol;

  // Init the data manager.
#ifdef DATABASE

  WRAP->DATA = DATA = new ychatdb;
#endif
  //*>>

  // Run threads
  //TIMR->run(); // TODO
}

#endif
