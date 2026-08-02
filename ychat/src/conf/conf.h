/*:*
 *: File: ./src/conf/conf.h
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

#ifndef CONF_H
#define CONF_H

class conf; // Predefine for nmap.tmpl

#include <map>
#include "../incl.h"
#include "../maps/shashmap.h"
#include "../name.h"
#include "../contrib/xml/tinyxml.h"

using namespace std;

class conf : public shashmap<string>, name
{
private:
  TiXmlDocument* p_xml;
  void exit_if_xml_error() const;
  void parse_xml( TiXmlNode* p_node, vector<string>* p_vec);

public:
  conf(string s_conf, map<string,string>* p_main_loop_params);
  ~conf();

  string colored_error_msg(string s_key);  //<<
  int get_int(string s_key);
  bool get_bool(string s_key);
  vector<string> get_vector(string s_key);
};

#endif
