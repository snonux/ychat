/*:*
 *: File: ./src/html.h
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

// class html declaration. this class manages the html-template files.
#include "incl.h"

#ifndef HTML_H
#define HTML_H

#include "maps/shashmap.h"
#include "chat/user.h"
#include "name.h"

using namespace std;

class html : public shashmap<string>, name
{
public:
  html( );
  ~html( );

  // Clears the template cache so that new html templates will be read
  // from hard disk. This method is needed after changeing s.t. on
  // the html-template files.
  void clear_cache( );

  // Returns a parsed html-template. this method will check first if the
  // required html-template exists inside the classes template cache. if not
  // then the file will be read from file and added to the cache.
  // afterwards the html-template will be parsed and returned.
  // map_params contains the client request parameters which also will be
  // used for string substituation.
  string parse( map<string,string> &map_params );

  void online_list( user *p_user, map<string,string> &map_params ); //<<
};

#endif
