/*:*
 *: File: ./src/tool/tool.cpp
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

#ifndef TOOL_CPP
#define TOOL_CPP

#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <fstream>
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
  // Execute the CGI file directly via fork/execve — NOT through a shell
  // (the old popen() ran `/bin/sh -c <s_command>`, so any shell
  // metacharacter in the URL-derived request path was a command-injection
  // / RCE vector when httpd.enablecgi=true). s_command must be the full
  // path to an executable file (the caller passes templatedir + request,
  // and reqp's path-traversal guard already prevents escaping templatedir).
  (void) m_method; // only METH_RETSTRING is used by the CGI path

  wrap::system_message(SHELLEX);
  wrap::system_message(s_command);

  struct stat st;
  if ( stat(s_command.c_str(), &st) != 0 || ! S_ISREG(st.st_mode) )
  {
    wrap::system_message( SHELLER );
    return "";
  }

  int fd[2];
  if ( pipe(fd) != 0 )
  {
    wrap::system_message( SHELLER );
    return "";
  }

  pid_t pid = fork();
  if ( pid < 0 )
  {
    close(fd[0]); close(fd[1]);
    wrap::system_message( SHELLER );
    return "";
  }

  if ( pid == 0 )
  {
    // child: wire stdout to the pipe and exec the file with an empty env.
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    // Don't let the CGI inherit the server's listen socket / other client
    // connections (a CGI shouldn't see or hold those fds).
    long l_maxfd = sysconf(_SC_OPEN_MAX);
    if ( l_maxfd < 0 ) l_maxfd = 256;
    for ( long i = STDERR_FILENO + 1; i < l_maxfd; ++i )
      close((int)i);
    char* argv[] = { const_cast<char*>(s_command.c_str()), (char*) NULL };
    char* envp[] = { (char*) NULL };
    execve( s_command.c_str(), argv, envp );
    _exit(127); // exec failed
  }

  // parent: read the CGI output.
  close(fd[1]);
  string s_ret;
  char buf[READBUF];
  for (;;)
  {
    ssize_t n = read(fd[0], buf, sizeof(buf));
    if ( n > 0 )
      s_ret.append( buf, n );
    else if ( n == 0 )
      break; // EOF
    else if ( errno == EINTR )
      continue;
    else
      break; // hard error
  }
  close(fd[0]);

  int i_status;
  waitpid( pid, &i_status, 0 );

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

// Returns true if the URL-decoded request path contains a "." or ".."
// path component (i.e. would escape the template directory when
// concatenated to httpd.templatedir and opened). The check must run on the
// DECODED path: the earlier raw-URL "/.." strip in sock.cpp misses
// %-encoded dots (e.g. %2e%2f) which decode to ".." afterwards.
bool
tool::path_has_traversal( const string &s_request )
{
  // Reject embedded NULs outright: url_decode turns %00 into '\0', and a
  // std::string can hold it while ifstream/c_str() truncate at it — a
  // divergence that is never legitimate in a request path.
  if ( s_request.find('\0') != string::npos )
    return true;

  size_t i_start = 0;
  size_t i_len   = s_request.size();

  for ( size_t i = 0; i <= i_len; ++i )
  {
    if ( i == i_len || s_request[i] == '/' )
    {
      string s_seg = s_request.substr( i_start, i - i_start );
      if ( s_seg == ".." || s_seg == "." )
        return true;
      i_start = i + 1;
    }
  }

  return false;
}

// Extracts the request target from the HTTP request line in s_buf
// ("<METHOD> <target> HTTP/x.y"), without its single leading '/', into
// s_target. Returns false for a request line without a method/target
// separator or without the " HTTP" version marker.
//
// The target starts right after the first space, whatever the method's
// length. The POST branch in sock.cpp used to hardcode substr(5, ...) for
// both "GET " (4 chars) and "POST " (5 chars), so a POST target kept its
// leading '/': "POST / HTTP/1.1" became request "/", which resolved to the
// template DIRECTORY itself, and the old html.cpp read loop spun forever on
// it (100% CPU, unbounded memory, OOMKill).
//
// Only the first line is searched so " HTTP" inside a header or POST body
// can never be mistaken for the version marker.
bool
tool::http_request_target( const string &s_buf, string &s_target )
{
  size_t i_line_end = s_buf.find( '\n' );
  string s_line = s_buf.substr( 0, i_line_end );

  size_t i_method_end = s_line.find( ' ' );
  if ( i_method_end == string::npos )
    return false;

  size_t i_http_pos = s_line.find( " HTTP", i_method_end );
  if ( i_http_pos == string::npos )
    return false;

  // i_http_pos == i_method_end means an empty target ("GET HTTP/1.1").
  size_t i_start = i_method_end + 1;
  s_target = i_http_pos > i_method_end
             ? s_line.substr( i_start, i_http_pos - i_start ) : "";

  if ( ! s_target.empty() && s_target[0] == '/' )
    s_target.erase( 0, 1 );

  return true;
}

// Reads the whole file s_path into s_content. Returns false (leaving
// s_content empty) unless s_path is a regular file of at most i_max_size
// bytes that could be read completely. Refusing directories and other
// non-regular files matters: an ifstream opened on a directory succeeds,
// but every read on it fails without ever setting eofbit, so an
// "until eof()" loop never terminates. The size bound keeps a huge or
// growing file from exhausting memory.
bool
tool::read_regular_file( const string &s_path, string &s_content,
                         size_t i_max_size )
{
  s_content.clear();

  struct stat st;
  if ( stat( s_path.c_str(), &st ) != 0 || ! S_ISREG( st.st_mode ) )
    return false;

  if ( static_cast<unsigned long long>( st.st_size ) > i_max_size )
    return false;

  ifstream if_file( s_path.c_str(), ios::binary );
  if ( ! if_file )
    return false;

  s_content.resize( st.st_size );
  if ( st.st_size > 0 && ! if_file.read( &s_content[0], st.st_size ) )
  {
    s_content.clear();
    return false;
  }

  return true;
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

