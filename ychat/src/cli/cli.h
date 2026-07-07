/*:*
 *: File: ./src/cli/cli.h
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <vector>

#ifndef	RUSAGE_SELF
#define RUSAGE_SELF 0
#endif
#ifndef	RUSAGE_CHILDREN
#define RUSAGE_CHILDREN -1
#endif

#include "../incl.h"

#ifndef CLI_H
#define CLI_H
#ifdef CLI

#include "../monitor/dump.h"

using namespace std;

class cli
{
private:
  int parse_input(string s_input);
  vector<string> vectorize(string s_param);

public:
  cli( );
  ~cli( );

#ifdef DEBUG

  void debug_routine();
#endif

  void print_rusage();
  void start(void* p_void);
  void start();
};

#endif
#endif
