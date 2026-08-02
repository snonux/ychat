/*:*
 *: File: ./src/sign.h
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

#ifndef SIGN_H
#define SIGN_H

#include "incl.h"
#include <sys/time.h>
#include <event.h>
#include <signal.h>

class sign
{
private:
  static struct event ev_sigpipe;
  static struct event ev_sigusr1;
  static struct event ev_sigusr2;
  static struct event ev_sighup;
  static struct event ev_sigint;
  static struct event ev_sigterm;
#ifdef CTCSEGV

  struct event ev_sigsev;
#endif

  static void handle_sigpipe(int i_fd, short event, void *p_arg);
  static void handle_sigusr1(int i_fd, short event, void *p_arg);
  static void handle_sigusr2(int i_fd, short event, void *p_arg);
  static void handle_sighup(int i_fd, short event, void *p_arg);
  static void handle_sigint(int i_fd, short event, void *p_arg);
  static void handle_sigterm(int i_fd, short event, void *p_arg);
#ifdef CTCSEGV

  static void handle_sigsev(int i_fd, short event, void *p_arg);
#endif

  static void clean_template_cache();
  static void reload_dlopen_modules(); //<<

public:
  static void init_event_handlers();
  static void terminate_received();
};

#endif
