/*:*
 *: File: ./src/reqp.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.4-CURRENT
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

#ifndef REQP_CPP
#define REQP_CPP

#include "reqp.h"
#include "tool/tool.h"

using namespace std;

#define HEADER HEADER1 HEADER2 HEADER3 HEADER4 HEADER9
#define STREAM HEADER5 HEADER6

const string reqp::s_http = HEADER;
const string reqp::s_http_stream = STREAM;
const string reqp::s_http_colength = HEADER7;
const string reqp::s_http_cotype = HEADER8;
const string reqp::s_http_cotype_add = HEADER8b;

reqp::reqp( )
{}

void
reqp::parse(context *p_context)
{
  map<string, string> &map_params = *p_context->p_map_params;

  string *p_response = p_context->p_response;
  string &s_event = map_params["event"];

  //<<*
  // check the event variable.
  if ( ! s_event.empty() )
  {
    // login procedure.
    if ( s_event == "login" )
    {
      wrap::CHAT->login( map_params );
    }
    else if ( s_event == "register" )
    {
      user* p_user = new user;
      map_params["INFO"] = "";
      run_html_mod( s_event, map_params, p_user );
      wrap::GCOL->add_user_to_garbage( p_user );
    }
    else
    {
      sess *p_sess = wrap::SMAN->get_session( map_params["tmpid"] );
      user *p_user = NULL;

      if ( p_sess != NULL )
      {
        p_user = p_sess->get_user();
      }
      else
      {
        // The session is invalid/expired (no tmpid match). The old code
        // returned here with an empty response, leaving the browser with a
        // blank page. Instead serve a small redirect page (redirect.html)
        // that does a *top-level* JS redirect back to the login page, so the
        // whole chat window returns to the login form rather than rendering
        // three stacked login forms inside the frameset's iframes.
        wrap::system_message(SESSERR);
        map_params["request"]      = "redirect.html";
        map_params["content-type"] = "text/html";

        // Build the full HTTP response here (mirroring the header wrapping
        // at the end of parse()) and return, so the normal template-render
        // path below -- which would re-render the originally-requested frame
        // (e.g. stream.html) -- doesn't run.
        *p_response = wrap::HTML->parse( map_params );

        string s_resp;
        s_resp.append( s_http );
        s_resp.append( s_http_colength + tool::int2string(p_response->size()) + "\r\n" +
                       s_http_cotype + map_params["content-type"] +
                       s_http_cotype_add + "\r\n" );
        s_resp.append( *p_response );
        *p_response = s_resp;
        return;
      }

      if ( ! p_user )
      {
        map_params["INFO"]    = wrap::CONF->get_elem( "chat.msgs.err.notonline" );
        map_params["request"] = wrap::CONF->get_elem( "httpd.startsite" ); // redirect to the startpage.
      }
      else
      {
        map_params["nick"] = p_user->get_name().c_str();

        // if a message input.
        if ( s_event == "input" )
        {
          if ( p_user )
          {
            p_user->check_restore_away();
            wrap::CHAT->post( p_user, map_params );
          }
        }

        // if a chat stream
        else if ( s_event == "stream" )
        {
          // Long-lived streaming chat-display connection. Build the initial
          // HTTP response (headers + the stream.html template) into p_response;
          // handle_client_read sends it via the keep-alive path and keeps the
          // connection open. Attach this fd to the user so msg_post can push
          // subsequent chat messages to it.
          string s_body = wrap::HTML->parse( map_params );

          string s_resp;
          s_resp.append( s_http ); // HTTP/1.1 200 OK ... Cache-Control ... Connection: close
          s_resp.append( s_http_cotype + map_params["content-type"] + s_http_cotype_add + "\r\n" );
          // No Content-Length: the body streams incrementally until close.
          s_resp.append( "\r\n" );
          s_resp.append( s_body );

          *p_response = s_resp;

          p_user->set_stream_fd( p_context->i_fd );
          p_context->p_user = p_user;
          map_params["KEEP_ALIVE"] = "yes";
          return;
        }

        // if a request for the online list of the active room.
        else if ( s_event == "online" )
        {
          wrap::HTML->online_list( p_user, map_params );
        }
        else //if ( s_event != "input" )
        {
          run_html_mod( s_event, map_params, p_user );
        }
      }
    }
  }

  if ( wrap::CONF->get_elem("httpd.enablecgi").compare("true") == 0 &&
       string::npos != map_params["request"].find(".cgi") )
  {
    p_response->append( tool::shell_command(
                          wrap::CONF->get_elem("httpd.templatedir") + map_params["request"],
                          METH_RETSTRING ) );
  }
  else
  {
    // parse and get the requested html-template and also use
    // the values stored in map_params for %%KEY%% substituations.
    p_response->append( wrap::HTML->parse( map_params ) );
  }

  // create the http header if not a stream
//  if ( s_event.compare("stream") != 0 ) {
  string s_resp("");
  s_resp.append(s_http);
  //s_resp.append( s_http_stream );
  s_resp.append( s_http_colength + tool::int2string(p_response->size()) + "\r\n" +
                 s_http_cotype + map_params["content-type"] +
                 s_http_cotype_add + "\r\n" );

  s_resp.append(*p_response);
  *p_response = s_resp;
// }
}

//<<*
void
reqp::run_html_mod( string s_event, map<string,string> &map_params, user* p_user )
{
  // Security: s_event is attacker-controlled (a query param) and is
  // concatenated into the html-module .so path then dlopen()'d. Reject
  // non-alphanumeric names so ".."/"/" can't traverse out of the modules dir
  // and load an arbitrary shared object (RCE).
  if ( ! tool::is_alpha_numeric(s_event) )
  {
    wrap::system_message("Reqp: blocked module-name traversal: " + s_event);
    return;
  }

  container *c = new container;

  c->elem[0] = (void*) wrap::WRAP;
  c->elem[1] = (void*) &map_params;
  c->elem[2] = (void*) p_user;

  string s_mod = wrap::CONF->get_elem("httpd.modules.htmldir") + "yc_" + s_event + ".so";

  dynmod* p_module = wrap::MODL->get_module( s_mod, p_user->get_name() );

  if ( p_module != NULL )
    ( *( p_module->the_func ) ) ( static_cast<void*>(c) );

  delete c;
}
//*>>

#endif
