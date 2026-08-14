/*:*
 *: File: ./src/conf/conf.cpp
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

#ifndef CONF_CPP
#define CONF_CPP

#include <fstream>
#include "conf.h"

using namespace std;

conf::conf( string s_conf, map<string,string>* p_main_loop_params ) : name::name( s_conf )
{
  string s_check[] =
  {
    get_name(),
    string(getenv("HOME"))+string("/.ychat/") + get_name(),
    string("./etc/") + get_name(),
    string("/etc/") + get_name(),
    string(PREFIX+string("etc/")+get_name())
  };

  string s_config;

  for ( int i = 0; i < 4; ++i )
  {
    cout << "Checking for " << s_check[i];
    ifstream if_check( s_check[i].c_str() );
    if ( if_check )
    {
      s_config = s_check[i];
      if_check.close();
      cout << "... ok!" << endl;
      break;
    }
    cout << "... not ok!" << endl;
  }

  if ( s_config.empty() )
  {
    cout << CFILEFA << endl;
    exit(1);
  }

  else
  {
    cout << CFILEOK << "..." << endl;
  }

  p_xml = new TiXmlDocument(s_config.c_str());
  TiXmlBase::SetCondenseWhiteSpace( false );

  if ( !p_xml->LoadFile() )
  {
    cout << XMLER1 << endl;
    exit(1);
  }

  vector<string> vec_string;
  parse_xml(p_xml, &vec_string);

  shashmap<string>::add_elem_insecure(tool::ychat_version(), "ychat.version");

  // Overrides ychat.conf values with command line options (ychat -o key1 value1 -o key2 value2 ...)
  map<string,string>::iterator iter;
  for ( iter = p_main_loop_params->begin(); iter != p_main_loop_params->end(); iter++ )
  {
    shashmap<string>::del_elem_insecure(iter->first);
    shashmap<string>::add_elem_insecure(iter->second, iter->first);
  }

  delete p_xml;
}

void
conf::parse_xml(TiXmlNode* p_node, vector<string>* p_vec)
{
  if ( p_node->NoChildren() )
    return;

  for ( TiXmlNode* p_child = p_node->FirstChild(); p_child; p_child = p_child->NextSibling() )
  {
    //cout << p_vec->size() << ": (Value:" << p_child->Value() << ") (Type:" << p_child->Type() << ")" << endl;

    if ( strcmp(p_child->Value(),"config") == 0 )
    {
      parse_xml(p_child, p_vec);
    }

    else if ( strcmp(p_child->Value(),"category") == 0 )
    {
      p_vec->push_back(p_child->ToElement()->Attribute("name"));
      parse_xml(p_child, p_vec);
      p_vec->pop_back();
    }

    else if ( strcmp(p_child->Value(),"option") == 0 )
    {
      string s_option_name = "";

      for ( vector<string>::iterator iter = p_vec->
                                            begin();
            iter != p_vec->end();
            ++iter )
        s_option_name.append(*iter + ".");

      TiXmlElement* p_element = p_child->ToElement();
      exit_if_xml_error();

      s_option_name.append(p_element->Attribute("name"));

#ifdef VERBOSE
      cout << XMLREAD << s_option_name;
#endif

      TiXmlNode *p_node_value = p_element->FirstChild("value");
      TiXmlNode *p_node_descr = p_element->FirstChild("descr");

      TiXmlText* p_text_value = NULL;
      TiXmlText* p_text_descr = NULL;

      if ( p_node_value && p_node_value->FirstChild()
         )
        p_text_value = p_node_value->FirstChild()->ToText();

      if ( p_node_descr && p_node_descr->FirstChild()
         )
        p_text_descr = p_node_descr->FirstChild()->ToText();

      if ( !p_text_value )
        continue;

#ifdef VERBOSE
      cout << " := " << p_text_value->Value() << endl;
#endif

      shashmap<string>::add_elem_insecure(p_text_value->Value(), s_option_name);

      if ( !p_text_descr )
        continue;

      s_option_name.append(".descr");
#ifdef VERBOSE
      cout << XMLREAD << s_option_name << endl;
#endif
      shashmap<string>::add_elem_insecure(p_text_descr->Value(), s_option_name);
    }
  }

  exit_if_xml_error()
  ;
}

conf::~conf()
{
  delete p_xml;
}

void
conf::exit_if_xml_error() const
{
  if ( p_xml->Error() )
  {
    cout << XMLERR << p_xml->ErrorDesc() << endl;
    exit(1);
  }
}

//<<*
string
conf::colored_error_msg(string s_key)
{
  return "<font color=\"#"
         + shashmap<string>::get_elem("chat.html.errorcolor")
         + "\">" + shashmap<string>::get_elem(s_key) + "</font><br>\n";
}
//*>>

int
conf::get_int(string s_key)
{
  return tool::string2int(get_elem(s_key));
}

bool
conf::get_bool(string s_key)
{
  return get_elem(s_key).compare("true") == 0;
}

vector<string>
conf::get_vector(string s_key)
{
  vector<string> vec_ret;
  string s_val = get_elem(s_key);

  for (size_type i_pos = s_val.find(" "); i_pos != string::npos; i_pos = s_val.find(" "))
  {
    vec_ret.push_back(s_val.substr(0, i_pos));
    s_val = s_val.substr(i_pos+1);
  }

  vec_ret.push_back(s_val);
  return vec_ret;
}
#endif
