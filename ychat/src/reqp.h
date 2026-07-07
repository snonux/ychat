/*:*
 *: File: ./src/reqp.h
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

#include "incl.h"
#ifndef REQP_H
#define REQP_H

#include "maps/hashmap.h"
#include "sock/context.h"

using namespace std;

class reqp
{
private:
  static const string s_http;
  static const string s_http_stream;
  static const string s_http_colength;
  static const string s_http_cotype;
  static const string s_http_cotype_add;

  void run_html_mod( string s_event, map<string,string> &map_params, user* p_user ); //<<

public:
  reqp( );
  void parse(context *p_context);
};

#endif
