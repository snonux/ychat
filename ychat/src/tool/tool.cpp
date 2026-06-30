/*:*
 *: File: ./src/tool/tool.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.0-CURRENT
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

#ifndef TOOL_CPP
#define TOOL_CPP

#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

#include "tool.h"

bool
tool::is_alpha_numeric( string &s_digit )
{
  const char *p_digit = s_digit.c_str();
  int   i_len = strlen( p_digit );

  for ( int i=0; i<i_len; i++ )
  {
    if ( ! isalnum( *p_digit ) )
      return false;
    p_digit++;
  }

  return true;
}

string
tool::int2string( int i_int )
{
  char buf[64];
  sprintf(buf, "%d", i_int);
  return buf;
}

string
tool::long2string( long l_long )
{
  char buf[64];
  sprintf(buf, "%d", l_long);
  return buf;
}

long
tool::unixtime()
{
  time_t clock;
  return (long) time( &clock );
}

int
tool::string2int( string s_digit )
{
  const char *p_digit = s_digit.c_str();
  int i_res = 0;

  // Convert each digit char and add into result.
  while (*p_digit >= '0' && *p_digit <='9')
  {
    i_res = (i_res * 10) + (*p_digit - '0');
    p_digit++;
  }

  // Check that there were no non-digits at end.
  if (*p_digit != 0)
  {
    return -1;
  }

  return i_res;
}

string
tool::to_lower( string s_str )
{
  string s_tmp("");

  for ( int i = 0; i < s_str.size() ;i++ )
    s_tmp = s_tmp + (char) tolower( s_str.at(i) );

  return s_tmp;
}

void
tool::strip_html( string *p_str)
{
  int i_pos;

  if ( (i_pos=p_str->find("<", 0)) == string::npos )
    return;

  while (true)
  {
    p_str->replace(i_pos, 1, "&lt;");

    if ( (i_pos = p_str->find("<", 0)) == string::npos )
      return;
  }
}

string
tool::ychat_version()
{
  return "yChat " + string(VERSION)
         + "-" + string(BRANCH)
         + " Build " + int2string(BUILDNR);
}

list<string>
tool::split_string(string s_string, string s_split)
{
  list<string> list_ret;
  unsigned i_pos, i_len = s_split.length();

  while ( (i_pos = s_string.find(s_split)) != (unsigned) string::npos )
  {
    list_ret.push_back( s_string.substr(0, i_pos) );
    s_string = s_string.substr( i_pos + i_len );
  }

  list_ret.push_back( s_string );

  return list_ret;
}

string
tool::trim( string s_str )
{
  // Left trim: find first non-whitespace char.
  size_t b = 0;
  while ( b < s_str.size() &&
         ( s_str[b] == ' ' || s_str[b] == '\n' || s_str[b] == '\r' ) )
    ++b;

  // Right trim: find last non-whitespace char.
  size_t e = s_str.size();
  while ( e > b &&
         ( s_str[e-1] == ' ' || s_str[e-1] == '\n' || s_str[e-1] == '\r' ) )
    --e;

  return s_str.substr( b, e - b );
}

char*
tool::clean_char( char* c_str )
{
  // Ralf:
  for ( char* c_pos = c_str; *c_pos != '\0'; ++c_pos )
    if ( iscntrl(*c_pos) )
      *c_pos = ' ';

  return c_str;
}

string
tool::replace( string s_string, string s_search, string s_replace )
{
  unsigned i_pos[2];

  for ( i_pos[0]  = s_string.find( s_search );
        i_pos[0] != (unsigned) string::npos;
        i_pos[0]  = s_string.find( s_search, i_pos[1] ) )
  {
    s_string.replace( i_pos[0], s_search.length(), s_replace );
    i_pos[1] = i_pos[0] + s_replace.length();
  }

  return s_string;
}

string
tool::get_extension( string s_file )
{
  int i_pos = s_file.find_last_of(".");

  if ( i_pos != string::npos )
  {
    string s_ext = s_file.substr(i_pos+1, s_file.size()-i_pos-1 );
    for ( int i = 0; i < s_ext.size(); ++i )
      s_ext[i] = tolower(s_ext[i]);

    return to_lower(s_ext);
  }

  return "";
}

char*
tool::int2char( int i_int )
{
  char *buf = new char[64];
  sprintf(buf, "%d", i_int);
  return buf;
}

string
tool::shell_command( string s_command, method m_method )
{
  FILE *file;
  char buf[READBUF];
  char *c_pos;
  string s_ret = "";

  wrap::system_message(SHELLEX);
  wrap::system_message(s_command);

  if ( (file=popen(s_command.c_str(), "r")) == NULL )
  {
    wrap::system_message( SHELLER );
  }
  else
  {
    while (true)
    {
      if (fgets(buf, READBUF, file) == NULL)
        break;

      switch (m_method)
      {
      default:
        s_ret.append("\n" + string(buf));
      } // switch
    }

    pclose(file);
  }

  return s_ret;
}

string
tool::url_decode( string s_url )
{
  string s_dest = "";
  int i_len = s_url.size();
  int i_prv = i_len - 2;

  char c;
  for ( int i = 0; i < i_len; ++i)
  {
    c = s_url.at(i);
    if ( c == '+' )
    {
      s_dest += " ";
    }
    else if (c == '%' && i < i_prv)
    {
      string s_tmp = s_url.substr(i+1, 2);
      c = (char) htoi(s_tmp);
      s_dest += c;
      i += 2;
    }
    else
    {
      s_dest += c;
    }
  }

  return s_dest;
}

int
tool::htoi(string &s_str)
{
  int value, c;
  c = s_str.at(0);

  if ( isupper(c) )
    c = tolower(c);

  value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

  c = s_str.at(1);

  if ( isupper(c) )
    c = tolower(c);

  value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

  return value;
}


#endif

