/*:*
 *: File: ./src/sign.cpp
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

#ifndef SIGN_CPP
#define SIGN_CPP

#include "sign.h"

struct event sign::ev_sigpipe;
struct event sign::ev_sigusr1;
struct event sign::ev_sigusr2;
struct event sign::ev_sighup;
struct event sign::ev_sigint;
struct event sign::ev_sigterm;
#ifdef CTCSEGV
struct event sign::ev_sigsev;
#endif

void
sign::clean_template_cache()
{
  wrap::HTML->clear_cache();
}

//<<*
void
sign::reload_dlopen_modules()
{
  wrap::MODL->reload_modules();
}
//*>>

void
sign::terminate_received()
{
  wrap::GCOL->remove_garbage(); //<<
  exit(0);
}

void
sign::handle_sigpipe(int i_fd, short event, void *p_arg)
{
  wrap::system_message(SIGNALR+string("SIGPIPE"));
}

void
sign::handle_sigusr1(int i_fd, short event, void *p_arg)
{
  wrap::system_message(SIGNALR+string("SIGUSR1"));
  clean_template_cache();
}

//<<*
void
sign::handle_sigusr2(int i_fd, short event, void *p_arg)
{
  wrap::system_message(SIGNALR+string("SIGUSR2"));
  reload_dlopen_modules();
}
//*>>

void
sign::handle_sighup(int i_fd, short event, void *p_arg)
{
  wrap::system_message(SIGNALR+string("SIGHUP"));
  terminate_received();
}

void
sign::handle_sigint(int i_fd, short event, void *p_arg)
{
  wrap::system_message(SIGNALR+string("SIGINT"));
  terminate_received();
}

void
sign::handle_sigterm(int i_fd, short event, void *p_arg)
{
  wrap::system_message(SIGNALR+string("SIGTERM"));
  terminate_received();
}

#ifdef CTCSEGV
void
sign::handle_sigsegv(int i_fd, short event, void *p_arg)
{
  wrap::system_message(SIGNALR+string("SIGSEGV"));
}
#endif


void
sign::init_event_handlers()
{
  // Ignore SIGPIPE. otherwise the server will shut down with "Broken pipe" if
  // a client unexpected disconnects himself from a SOCK_STREAM.
  signal_set(&ev_sigpipe, SIGPIPE, handle_sigpipe, NULL);
  signal_add(&ev_sigpipe, NULL);

  signal_set(&ev_sigusr1, SIGUSR1, handle_sigusr1, NULL);
  signal_add(&ev_sigusr1, NULL);

  //<<*
  signal_set(&ev_sigusr2, SIGUSR2, handle_sigusr2, NULL);
  signal_add(&ev_sigusr2, NULL);
  //*>>

  signal_set(&ev_sighup, SIGHUP, handle_sighup, NULL);
  signal_add(&ev_sighup, NULL);

  signal_set(&ev_sigint, SIGINT, handle_sigint, NULL);
  signal_add(&ev_sigint, NULL);

  signal_set(&ev_sigterm, SIGTERM, handle_sigterm, NULL);
  signal_add(&ev_sigterm, NULL);

#ifdef CTCSEGV

  signal_set(&ev_sigsev, SIGSEGV, handle_sigsev, NULL);
#endif

}

#endif
