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

#include "tool.h"

bool
tool::is_alpha_numeric( string &s_digit )
{
  const char *p_digit = s_digit.c_str();
  int   i_len = strlen( p_digit );

  for( int i=0; i<i_len; i++ )
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

  for( int i = 0; i < s_str.size() ;i++ )
    s_tmp = s_tmp + (char) tolower( s_str.at(i) );

  return s_tmp;
}

void
tool::strip_html( string *p_str)
{
  int i_pos;

  if( (i_pos=p_str->find("<", 0)) == string::npos )
    return;

  while(true)
  {
    p_str->replace(i_pos, 1, "&lt;");

    if( (i_pos = p_str->find("<", 0)) == string::npos )
      return;
  }
}

string
tool::yhttpd_version()
{
  return "yhttpd " + string(VERSION)
         + "-" + string(BRANCH)
         + " Build " + int2string(BUILDNR);
}

list<string>
tool::split_string(string s_string, string s_split)
{
  list<string> list_ret;
  size_t i_pos, i_len = s_split.length();

  while ( (i_pos = s_string.find(s_split)) != string::npos )
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
  size_t i_pos[2];

  for ( i_pos[0]  = s_string.find( s_search );
        i_pos[0] != string::npos;
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

  if( i_pos != string::npos )
  {
    string s_ext = s_file.substr(i_pos+1, s_file.size()-i_pos-1 );
    for( int i = 0; i < s_ext.size(); ++i )
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
  // Execute the CGI file directly via fork/execve - NOT through a shell.
  // The old popen() ran `/bin/sh -c <s_command>`, so any shell metacharacter
  // in the URL-derived request path was a command-injection / RCE vector
  // when httpd.enablecgi=true. s_command must be the full path to an
  // executable file (reqp's remove_dots already prevents ".." escaping the
  // template dir, and stat() rejects non-files).
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
    // child: stdout -> pipe, close inherited fds, exec the file (no shell).
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    long l_maxfd = sysconf(_SC_OPEN_MAX);
    if ( l_maxfd < 0 ) l_maxfd = 256;
    for ( long i = STDERR_FILENO + 1; i < l_maxfd; ++i )
      close((int)i);
    char* argv[] = { const_cast<char*>(s_command.c_str()), (char*) NULL };
    char* envp[] = { (char*) NULL };
    execve( s_command.c_str(), argv, envp );
    _exit(127);
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
      break;
    else if ( errno == EINTR )
      continue;
    else
      break;
  }
  close(fd[0]);

  int i_status;
  waitpid( pid, &i_status, 0 );

  return s_ret;
}

#endif

