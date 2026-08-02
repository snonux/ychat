/*:*
 *: File: ./src/mods/commands/yc_getrusage.cpp
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include "../../chat/user.h"

#ifndef	  RUSAGE_SELF
#define   RUSAGE_SELF 0
#endif
#ifndef	  RUSAGE_CHILDREN
#define   RUSAGE_CHILDREN -1
#endif

/*
 gcc -shared -o yc_name.so yc_name.cpp
*/

using namespace std;

extern "C"
{
  int valid_color( string );
  int extern_function(void *v_arg)
  {
    container *c=(container *)v_arg;

    user *p_user = (user*) c->elem[1];		// the corresponding user
    rusage* p_rusage = new rusage;
    getrusage( RUSAGE_SELF, p_rusage );
    char c_msg[1024];

    /* see man getrusage  */

    sprintf(c_msg, "getrusage:<br>\nmaxrss: %D (max resident set size)<br>\nixrss: %D (integral shared text memory size)<br>\nidrss: %D (integral unshared data size)<br>\nisrss %D (integral unshared stack size)<br>\nminflt: %D (page reclaims)<br>\nmajflt: %D (page faults)<br>\nnswap: %D (swaps)<br>\ninblock: %D (block input operations)<br>\noublock: %D (block output operations)<br>\nmsgsnd: %D (messages sent)<br>\nmsgrcv: %D (messages received)<br>\nnsignals: %D (signals received)<br>\nnvcsw: %D (voluntary context switches)<br>\nnivcsw: %D (involuntary context switches)<br>\n",
            p_rusage->ru_maxrss,
            p_rusage->ru_ixrss,
            p_rusage->ru_idrss,
            p_rusage->ru_isrss,
            p_rusage->ru_minflt,
            p_rusage->ru_majflt,
            p_rusage->ru_nswap,
            p_rusage->ru_inblock,
            p_rusage->ru_oublock,
            p_rusage->ru_msgsnd,
            p_rusage->ru_msgrcv,
            p_rusage->ru_nsignals,
            p_rusage->ru_nvcsw,
            p_rusage->ru_nivcsw
           );

    p_user->msg_post( new string( c_msg ) );

    return 0;
  }
}

