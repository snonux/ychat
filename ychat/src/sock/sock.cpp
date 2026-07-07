/*:*
 *: File: ./src/sock/sock.cpp
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

#ifndef SOCK_CPP
#define SOCK_CPP

#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

#include "sock.h"
#include "../tool/tool.h"
#include "context.h"

using namespace std;

vector< sock* > sock::vec_socks;
int sock::i_id_counter = 0;

sock::sock()
{
  this->i_id = ++i_id_counter;

  this->b_run      = true;
  this->i_req      = 0;
  this->p_reqp = new reqp();

#ifdef LOGGING
  this->log_daemon = new logd( wrap::CONF->get_elem( "httpd.logging.accessfile" ),
                               wrap::CONF->get_elem( "httpd.logging.access_lines" ) );
#endif

  this->p_ip_cache_map = new shashmap < string, unsigned, self_hash<unsigned>, equals_allocator<unsigned> >;
  vec_socks.push_back(this);
}

//<<*
/*
void
sock::chat_stream(context *p_context, string s_msg)
{
  p_user->set_context(p_sock);

  _send(p_sock, s_msg.c_str(), s_msg.size()); // FOOBAR

  for (int i = 0; i < PUSHSTR; ++i)
    _send(p_sock, " \n", 2);

  p_user->set_sock(p_sock);
}
*/
//*>>

int
sock::_make_server_socket( int i_port )
{
  size_t i_sock;
  struct sockaddr_in name;

  // create the server socket.
  i_sock = socket (AF_INET, SOCK_STREAM, 0);
  // i_sock = socket (PF_INET, SOCK_STREAM, 0);
  if (i_sock < 0)
  {
    wrap::system_message(SOCKERR, errno);

    if ( ++i_port > MAXPORT )
      exit(1);

    wrap::system_message(SOCKERR, errno);

    return _make_server_socket( i_port );
  }

  // give the server socket a name.
  name.sin_family = AF_INET;
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  name.sin_port = htons(i_port);
  int i_optval = 1;

  // SO_REUSEADDR must be set BEFORE bind, otherwise a quick container
  // restart can hit EADDRINUSE on port 2000 (TIME_WAIT) and fall back to
  // 2001, which the k8s Service doesn't target.
  setsockopt(i_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&i_optval, sizeof(int));

  if (bind(i_sock, (struct sockaddr *) &name, sizeof (name)) < 0)
  {
    wrap::system_message(BINDERR, errno);

    if (++i_port > MAXPORT)
      exit(1);

    wrap::system_message(string(SOCKERR) + tool::int2string(i_port));

    // Re-run recursive.
    return _make_server_socket(i_port);
  }

  i_server_port = i_port;
  i_server_sock = i_sock;

  if (set_nonblock(i_server_sock) < 0)
  {
    wrap::system_message(NONBLER, errno);
    exit(EXIT_FAILURE);
  }

  wrap::system_message(SOCKCRT + string("localhost:") + tool::int2string(i_port));

  if (listen(i_sock, BACKLOG) < 0)
  {
    wrap::system_message(LISTERR, errno);
    exit(EXIT_FAILURE);
  }

  wrap::system_message(SOCKRDY);
  return i_sock;
}

#ifdef OPENSSL
// This method is virtual, and is overloaded by sslsock!
bool
sock::_main_loop_do_ssl_stuff(int &i_new_sock)
{
  return 0;
}
#endif

void
sock::process_request()
{
  int i;
  struct sockaddr_in clientname;
  socklen_t size = sizeof(clientname);

  ++i_req;
  int i_client_sock = accept(i_server_sock, (struct sockaddr *) &clientname, &size);

  if (i_client_sock == -1)
  {
    switch (errno)
    {
    case EAGAIN:
    case EINTR:
      return;
    default:
      // Any other accept error: bail out for this event rather than
      // proceed with an invalid fd (EBADF in set_nonblock + UB downstream).
      wrap::system_message(SOCKER5, errno);
      return;
    }
  }

  if (0 < set_nonblock(i_client_sock))
  {
    wrap::system_message(NONBCER, errno);
  }

#ifdef OPENSSL
  _main_loop_do_ssl_stuff(i_client_sock);
#endif

#ifdef VERBOSE
  wrap::system_message(NEWREQU
                       + tool::int2string(i_req) +  " "
                       + string(inet_ntoa( clientname.sin_addr )) + ":"
                       + tool::int2string(ntohs ( clientname.sin_port ))
                       + " @" + tool::int2string(get_id())
                      );
#endif

  context *p_context = new context(this, new struct event, i_client_sock);

  event_set(p_context->p_event, i_client_sock, EV_READ , handle_client_read, p_context);
  event_add(p_context->p_event, NULL);
}

void
sock::clean_ipcache()
{
  int i_ipcachesize = wrap::CONF->get_int("httpd.ipcachesize");
  int i_currentsize = p_ip_cache_map->size();

  if (i_currentsize > 0 && (i_ipcachesize == 0 || i_ipcachesize <= i_currentsize))
    wrap::system_message(SOCKCA2+tool::int2string(i_currentsize)+","+tool::int2string(i_ipcachesize)+")");
}

void
sock::init_event_handlers()
{
  for (vector< sock* >::iterator iter = sock::vec_socks.begin();
       iter != sock::vec_socks.end(); ++iter)
  {
    wrap::system_message(EVENTSO, (*iter)->get_id());
    (*iter)->init_server_event_handler();
  }
}

void
sock::init_server_event_handler()
{
  event_set(&ev_handle_server_accept, i_server_sock, EV_READ | EV_PERSIST, handle_server_accept, this);
  event_add(&ev_handle_server_accept, NULL);
}

void
sock::handle_server_accept(int i_fd, short event, void *p_arg)
{
  sock *p_sock = static_cast<sock*>(p_arg);
  p_sock->process_request();
}

void
sock::handle_client_read(int i_fd, short event, void *p_arg)
{
  static int i_size = READSOCK * sizeof(char);
  context *p_context = static_cast<context*>(p_arg);

  // A request (especially a POST body) can arrive split across several TCP
  // segments — e.g. when a reverse proxy (Traefik) forwards headers and body
  // separately. Accumulate into c_buf across re-arms of this event until we
  // have the full request, then process it.
  bool b_got_data = false;
  for (;;)
  {
    if ( p_context->i_buf_len >= READSOCK )
      break; // buffer full

    ssize_t n = read(i_fd, p_context->c_buf + p_context->i_buf_len,
                     i_size - p_context->i_buf_len);

    if (n > 0)
    {
      p_context->i_buf_len += n;
      b_got_data = true;
      continue; // drain whatever else is currently buffered
    }

    if (n == 0)
    {
      // EOF: client closed before a complete request. Drop it.
      if (!b_got_data || p_context->i_buf_len == 0)
      {
        p_context->del_event();
        delete p_context;
        return;
      }
      break; // process whatever we did get
    }

    // n < 0
    if (errno == EAGAIN || errno == EINTR)
    {
      if (!b_got_data)
      {
        // Nothing new this invocation; wait for more.
        event_add(p_context->p_event, NULL);
        return;
      }
      break; // we have some data; check below whether the request is complete
    }

    // Hard read error.
    p_context->del_event();
    delete p_context;
    return;
  }

  p_context->c_buf[p_context->i_buf_len] = '\0';
  string s_buf(p_context->c_buf);

  // For POST, wait until we have the full body (per Content-Length) before
  // parsing — otherwise the form params (event/nick/tmpid/...) are missing
  // and logins silently no-op.
  if ( strncmp("POST", p_context->c_buf, 4) == 0 )
  {
    size_t i_hdr_end = s_buf.find("\r\n\r\n");
    size_t i_body_off = string::npos;
    if (i_hdr_end != string::npos)
      i_body_off = i_hdr_end + 4;
    else
    {
      i_hdr_end = s_buf.find("\n\n");
      if (i_hdr_end != string::npos)
        i_body_off = i_hdr_end + 2;
    }

    if (i_hdr_end == string::npos)
    {
      // Headers not fully received yet. If the buffer is already full with
      // no header terminator (oversized header flood) the request can never
      // complete: drop it now (same pattern as the EOF/error branches) rather
      // than spin. Otherwise wait for the rest.
      if ( p_context->i_buf_len < READSOCK )
        event_add(p_context->p_event, NULL);
      else
      {
        p_context->del_event();
        delete p_context;
      }
      return;
    }

    size_t i_cl = s_buf.find("Content-Length:");
    if (i_cl != string::npos)
    {
      size_t v = i_cl + 16;
      size_t ve = s_buf.find_first_of("\r\n", v);
      if (ve == string::npos) ve = s_buf.size();
      int i_content_len = atoi(s_buf.substr(v, ve - v).c_str());
      int i_have = (int)s_buf.size() - (int)i_body_off;
      if (i_have < i_content_len)
      {
        // Body still incomplete. If the buffer is already full the body
        // can never fit (oversized POST): drop it now (same pattern as the
        // EOF/error branches) rather than spin. Otherwise wait for the rest.
        if ( p_context->i_buf_len < READSOCK )
          event_add(p_context->p_event, NULL);
        else
        {
          p_context->del_event();
          delete p_context;
        }
        return;
      }
    }
  }

  p_context->del_event();

  string s_query("");

  bool b_is_post_request;

  if (strncmp("POST", p_context->c_buf, 4) == 0)
  {
    b_is_post_request = true;;

    // Crash fix: this used to compute `s_buf.find(" HTTP", 0) + 1` straight
    // into an int. When " HTTP" isn't present (e.g. a scanner sending a bare
    // "GET\r\n\r\n" with no path/version), find() returns string::npos and the
    // "+ 1" wraps a 64-bit npos to 0 before it's ever compared, so the
    // intended "== string::npos" guard below never fires. Execution then
    // fell through to substr(5, ...), which throws std::out_of_range (an
    // uncaught exception that kills the process) whenever the buffer is
    // shorter than 5 bytes. Checking find()'s result before adding 1 -- and
    // requiring at least 5 bytes to extract from -- catches both the missing
    // " HTTP" and the too-short-buffer case as an ordinary invalid request.
    size_t i_http_pos = s_buf.find(" HTTP", 0);

    if (i_http_pos == string::npos || s_buf.size() < 5)
    {
      wrap::system_message(HTTPERR);
      delete p_context;
      return;
    }

    // Use i_http_pos (not i_http_pos+1) as the length so the space before
    // "HTTP/1.1" is NOT included in s_query. A trailing space here used to
    // leak into the last query parameter (e.g. tmpid=...<space>) and break
    // session lookups for GET/stream requests whose tmpid is the last param.
    s_query.append(s_buf.substr(5, i_http_pos - 5));
  }

  else if (strncmp("GET", p_context->c_buf, 3) == 0)
  {
    b_is_post_request = false;
    // Same npos-overflow/short-buffer crash fix as the POST branch above.
    size_t i_http_pos = s_buf.find(" HTTP", 0);

    if (i_http_pos == string::npos || s_buf.size() < 5)
    {
      wrap::system_message(HTTPERR);
      delete p_context;
      return;
    }

    // See POST branch above: exclude the space before "HTTP/1.1" so it
    // doesn't leak into the last query parameter (breaks session lookups for
    // stream/online requests whose tmpid is the last parameter).
    s_query.append(s_buf.substr(5, i_http_pos - 5));
  }

  // Invalid request
  else
  {
    wrap::system_message(HTTPERR);
    delete p_context;
    return;
  }

  // Remove /.. from the query
  int i_pos, i_pos2;
  while ( (i_pos = s_query.find("/..")) != string::npos )
    s_query.replace(i_pos, 3, "/");

  sock *p_sock = p_context->p_sock;

  p_context->p_map_params = new map<string, string>;
  map<string, string>& map_params = *p_context->p_map_params;

  struct sockaddr_in client;
  static socklen_t client_size = sizeof(client);
  getpeername(i_fd, (struct sockaddr *) &client, &client_size);

  shashmap< string, unsigned, self_hash<unsigned>, equals_allocator<unsigned> > *p_ip_cache_map
  = p_sock->get_ip_cache_map();

  unsigned &i_addr = client.sin_addr.s_addr;

  if ( (map_params["REMOTE_ADDR"] = p_ip_cache_map->get_elem(i_addr)) == "" )
  {
    map_params["REMOTE_ADDR"] = string(inet_ntoa(client.sin_addr));
    p_ip_cache_map->set_elem(map_params["REMOTE_ADDR"], i_addr);
    wrap::system_message(SOCKCAC+map_params["REMOTE_ADDR"]);
  }

  // Get HTTP Header values
  i_pos = s_buf.find("\n");
  if (i_pos != string::npos)
  {
    map_params["QUERY_STRING"] = tool::trim(s_buf.substr(0,i_pos-1));

    do
    {
      string s_line(s_buf.substr(0, i_pos));
      i_pos2 = s_line.find(":");

      if (i_pos2 != string::npos && s_line.length() > i_pos2+1)
        map_params[ tool::trim(s_line.substr(0, i_pos2)) ] = tool::trim(s_line.substr(i_pos2+1));

      s_buf = s_buf.substr(s_line.size() + 1);
      i_pos = s_buf.find("\n");

    }
    while (i_pos != string::npos);
  }

  // Get request string parameters
  string s_parameters = tool::url_decode(s_query);
  string s_tmp, s_request("");

  if ( (i_pos = s_parameters.find("?")) != string::npos)
  {
    s_request.append(s_parameters.substr(0, i_pos));
    s_parameters = s_parameters.substr(i_pos+1);
  }

  else if ( (i_pos = s_parameters.find(" ")) != string::npos)
  {
    s_request.append(s_parameters.substr(0, i_pos));
  }

  else
  {
    s_request.append(s_parameters);
  }

  if (b_is_post_request)
    // The POST body arrives URL-encoded (browsers encode spaces as '+'
    // and '/' etc. as %XX). The GET path URL-decodes s_query up front (see
    // the url_decode() call above); the POST body must be decoded the same
    // way, otherwise a chat command like "/col 0000FF FF0000" arrives as
    // "%2Fcol+0000FF+FF0000" and is never recognized as a command (the '/'
    // is hidden behind %2F), so e.g. saving colors silently no-ops.
    s_parameters = tool::url_decode(s_buf);

  while ( (i_pos = s_parameters.find("&")) != string::npos )
  {
    s_tmp = s_parameters.substr(0, i_pos );

    if ( (i_pos2 = s_tmp.find("=")) != string::npos )
      map_params[ s_tmp.substr(0, i_pos2) ] = tool::replace( s_tmp.substr( i_pos2+1 ), "\\AND", "&");

    s_parameters = s_parameters.substr(i_pos+1);
  }

  // Get the last request parameter, which does not have a "&" on the end!
  // Take the full value after '=' (matching the while-loop branch above).
  // The previous code truncated the value at the first space, which dropped
  // everything after the first word of any multi-word value -- so a POSTed
  // "message=/col 0000FF FF0000" became "message=/col" and the /col command
  // lost its color arguments (falling back to the default colors).
  if ( (i_pos = s_parameters.find("=")) != string::npos )
    map_params[ s_parameters.substr(0, i_pos) ] =
      tool::replace( s_parameters.substr(i_pos+1), "\\AND", "&");

#ifdef VERBOSE
  wrap::system_message(REQUEST + s_request);
#endif

  if (s_request.empty())
    s_request = wrap::CONF->get_elem("httpd.startsite");

  // Path-traversal guard (security): reject any request whose decoded path
  // contains a "."/".." component — it would escape httpd.templatedir when
  // html.cpp opens (templatedir + request). The raw-URL "/.." strip above
  // is insufficient because it runs before url_decode (attacker uses %2e%2f).
  if ( tool::path_has_traversal(s_request) )
  {
    wrap::system_message("Sock: blocked path traversal: " + s_request);
    s_request = wrap::CONF->get_elem("httpd.html.notfound");
  }

  map_params["request"] = s_request;

  {
    string s_ext(tool::get_extension(s_request));

    if ( s_ext == "" )
      s_ext = "default";

    map_params["content-type"] = wrap::CONF->get_elem( "httpd.contenttypes." + s_ext );
  }

//#ifdef VERBOSE
//  for (map<string, string>::iterator i = map_params.begin(); i != map_params.end(); ++i)
//	  cout << "=>" << i->first << "=" << i->second << "<=" << endl;
//#endif

  p_context->p_response = new string("");
  p_sock->get_req_parser()->parse(p_context);

  if ( (*p_context->p_map_params)["KEEP_ALIVE"] == "yes" )
  {
    // Streaming chat-display connection: send the initial response, then
    // keep the fd open for pushed messages and watch for disconnect.
    struct event *p_ev = new struct event;
    p_context->p_event = p_ev;
    event_set( p_ev, i_fd, EV_WRITE, handle_stream_write, p_context );
    event_add( p_ev, NULL );
    return;
  }

  struct event *p_ev_handle_client_write = new struct event;
  p_context->p_event = p_ev_handle_client_write;

  event_set(p_ev_handle_client_write, i_fd, EV_WRITE, handle_client_write, p_context);
  event_add(p_ev_handle_client_write, NULL);
}

// Finish sending the initial stream response, then switch to a persistent
// read event that detects the client disconnecting. Does NOT delete the
// context (the connection stays open for streamed chat messages).
void
sock::handle_stream_write(int i_fd, short event, void *p_arg)
{
  context *p_context = static_cast<context*>(p_arg);
  string *p_response = p_context->p_response;

  if ( p_response && ! p_response->empty() )
  {
    ssize_t n = write( i_fd, p_response->data(), p_response->size() );
    if ( n > 0 )
      p_response->erase( 0, n );
    else if ( n < 0 && ( errno == EAGAIN || errno == EINTR ) )
    {
      event_add( p_context->p_event, NULL );
      return;
    }
    else if ( n < 0 )
    {
      // Peer gone before the initial response was sent.
      if ( p_context->p_user )
        p_context->p_user->clear_stream();
      delete p_context;
      return;
    }

    if ( ! p_response->empty() )
    {
      event_add( p_context->p_event, NULL );
      return;
    }
  }

  // Initial response fully sent: free it, mark the user's stream as ready
  // to receive pushed messages, flush anything already buffered, then arm
  // a persistent read event to detect disconnect.
  delete p_response;
  p_context->p_response = NULL;

  if ( p_context->p_user )
  {
    p_context->p_user->set_stream_ready( true );
    p_context->p_user->flush_stream();
  }

  event_set( p_context->p_event, i_fd, EV_READ | EV_PERSIST, handle_stream_read, p_context );
  event_add( p_context->p_event, NULL );
}

// The stream connection is quiet except for disconnect. Drain any stray
// bytes the client sends; on EOF/error reap the context and clear the
// user's stream reference.
void
sock::handle_stream_read(int i_fd, short event, void *p_arg)
{
  context *p_context = static_cast<context*>(p_arg);
  char c_buf[256];

  for (;;)
  {
    ssize_t n = read( i_fd, c_buf, sizeof(c_buf) );
    if ( n > 0 )
      continue; // drain unexpected client data
    if ( n == 0 )
      break;    // EOF -> disconnect
    // n < 0
    if ( errno == EAGAIN || errno == EINTR )
      return;   // still connected; persistent event stays armed
    break;      // error -> disconnect
  }

  if ( p_context->p_event )
    event_del( p_context->p_event );
  if ( p_context->p_user )
  {
    // Reap the user from its room (no-op if already offline, e.g. idle-reaped).
    // Without this a cleanly-disconnected online user would linger in the
    // room/online list until the next idle-timeout sweep.
    p_context->p_user->set_online(false);
    p_context->p_user->clear_stream();
  }
  delete p_context;
}

void
sock::handle_client_write(int i_fd, short event, void *p_arg)
{
  static int i_char_size = sizeof(char);

  context *p_context = static_cast<context*>(p_arg);
  string *p_response = p_context->p_response;

  if (-1 == write(i_fd, p_response->c_str(), p_response->length()*i_char_size))
  {
    switch (errno)
    {
    case EAGAIN:
    case EINTR:
      event_add(p_context->p_event, NULL);
      return;
    }
  }

  delete p_context;
}

int
sock::set_nonblock(int i_sock)
{
  if (fcntl(i_sock, F_SETFL, O_NONBLOCK) < 0)
  {
    wrap::system_message(SOCKER5, errno);
    return -1;
  }

  return 0;
}

#endif
