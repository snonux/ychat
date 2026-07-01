
#ifndef LOGD_CPP
#define LOGD_CPP

#include "logd.h"

#ifdef LOGGING

#include <fstream>
#include <iostream>
#include <cstring>
#include <cerrno>

logd::logd( string s_filename, string s_log_lines )
{
  initialize( s_filename, tool::string2int(s_log_lines) );
}

logd::logd( string s_filename, int i_log_lines )
{
  initialize( s_filename, i_log_lines );
}

logd::~logd()
{
  flush_logs();
  pthread_mutex_destroy( &mut_s_logging );
}

void
logd::initialize( string s_filename, int i_log_lines )
{
  pthread_mutex_init( &mut_s_logging, NULL );

  if( s_filename.empty() )
  {
    wrap::system_message( LOGERR2 );
    exit(1);
  }

  //if (wrap::NCUR->is_ready())
  //  wrap::system_message(LOGGINI+s_filename);

  s_logfile = s_filename;
  i_lines   = i_log_lines;
}

string
logd::get_time_string()
{
  struct tm *t_m;
  time_t t_cur = time(NULL);
  t_m = gmtime(&t_cur);

  char c_buf[100];
  c_buf[99] = '\0';
  strftime(c_buf, 100, "[%d/%b/%Y:%H:%M:%S %z]", t_m);

  return string(c_buf);
}

void
logd::flush()
{
  ofstream of_output;
  of_output.open(s_logfile.c_str(), ios::app);

  if ( ! of_output.is_open() )
  {
    // Log to stderr only; do NOT call wrap::system_message() - that routes
    // back through LOGD->log_simple_line -> flush on the same failing logd
    // and would recurse infinitely (stack overflow).
    cerr << "yhttpd: could not open logfile '" << s_logfile << "' ("
         << strerror(errno) << ")" << endl;
    exit(1);
  }

  while( ! s_queue.empty() )
  {
    string s_l=s_queue.front();
    s_queue.pop();
    of_output.write( s_l.c_str(), s_l.size() );
  }

  of_output.close();
}

void
logd::log_access( map<string,string> &map_request )
{
  //static int i_access_lines = wrap::CONF->get_elem("httpd.logging.access_lines");

  string s_time = get_time_string();
  string s_logstr = map_request["REMOTE_ADDR"] + " - - "+s_time+" \"" + map_request["QUERY_STRING"]+"\" 200 0 \""+map_request["request"]+"\" \""+map_request["User-Agent"]+"\"\n";

  pthread_mutex_lock ( &mut_s_logging );
  s_queue.push(s_logstr);

  if ( s_queue.size() > i_lines )
    flush();

  pthread_mutex_unlock( &mut_s_logging );
}

void
logd::log_simple_line( string s_line )
{
  // Dont log empty lines!
  if (s_line.empty())
    return;

  string s_time = get_time_string();
  string s_logstr = s_time + " " + s_line;

  pthread_mutex_lock  ( &mut_s_logging );
  s_queue.push(s_logstr);

  if ( s_queue.size() > i_lines )
    flush();

  pthread_mutex_unlock( &mut_s_logging );
}

void
logd::set_logfile( string s_path, string s_filename )
{
  // Remove "/" from filename!
  size_t i_pos = s_filename.find( "/" );
  while ( i_pos != string::npos )
  {
    s_filename.replace( i_pos, 1, "SLASH" );
    i_pos = s_filename.find( "/" );
  }

  // Remove "\" from filename (for non unix systems)!
  i_pos = s_filename.find( "\\" );
  while ( i_pos != string::npos )
  {
    s_filename.replace( i_pos, 1, "BACKSLASH" );
    i_pos = s_filename.find( "\\" );
  }

  pthread_mutex_lock  ( &mut_s_logging );
  this->s_logfile = s_path + s_filename;
  pthread_mutex_unlock( &mut_s_logging );
}

void
logd::flush_logs()
{
  pthread_mutex_lock  ( &mut_s_logging );
  flush();
  pthread_mutex_unlock( &mut_s_logging );
}

string
logd::remove_html_tags( string s_logs )
{
  size_t pos[2];

  while ( (pos[0] = s_logs.find("<")) != string::npos )
  {
    if ( (pos[1] = s_logs.find(">", pos[0])) != string::npos )
      s_logs.replace( pos[0], pos[1]-pos[0]+1, "");
    else
      break;
  }

  if ( s_logs == "\n" )
    return "";

  return s_logs;
}

void
logd::set_lines( const int i_lines )
{
  this->i_lines = i_lines;
}

#endif
#endif
