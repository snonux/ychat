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
reqp::get_request_parameters( string s_parameters, map<string,string>& map_params )
{
  string s_tmp;
  size_t i_pos, i_pos2;

  while( (i_pos = s_parameters.find("&")) != string::npos )
  {
    s_tmp = s_parameters.substr(0, i_pos );

    if ( (i_pos2 = s_tmp.find("=")) != string::npos )
      map_params[ s_tmp.substr(0, i_pos2) ] = tool::replace( s_tmp.substr( i_pos2+1 ), "\\AND", "&");

    s_parameters = s_parameters.substr( i_pos + 1 );
  }

  // Get the last request parameter, which does not have a "&" on the end!
  if( (i_pos = s_parameters.find("=")) != string::npos )
    map_params[ s_parameters.substr(0, i_pos) ] = s_parameters.substr( i_pos+1 );

  //map<string,string>::iterator iter;
  //for ( iter = map_params.begin(); iter != map_params.end(); ++iter )
  //cout << ">>>" << iter->first << "=" << iter->second << endl;
}

string
reqp::get_url( string s_req, map<string, string> &map_params, int& i_postpayloadoffset )
{
  size_t i_pos, i_pos2;
  string s_vars( "" );
  string s_ret;
  int i_req;

  // GET request
  if ( s_req.find("GET") != string::npos)
  {
    // Be sure that the GET request has minimum length
    if ( s_req.length() > 5 )
    {
      // Get rid of "GET /"
      if ( (i_pos = s_req.find("\n")) == string::npos )
        i_pos = s_req.length() - 1;

      s_req = s_req.substr(5, i_pos - 5);

      // Get HTML site to be displayed
      if ( (i_pos = s_req.find("?")) == string::npos )
      {
        if ( (i_pos2 = s_req.find(" HTTP")) != string::npos )
          s_ret = url_decode( s_req.substr(0, i_pos2));
      }
      else
      {
        s_ret = url_decode( s_req.substr(0, i_pos) );

        // Get request parameters:
        if ( (i_pos2 = s_req.find(" HTTP")) != string::npos )
        {
          s_req = url_decode( s_req.substr(i_pos + 1, i_pos2 - i_pos - 1) );
          get_request_parameters( s_req, map_params );
        }
      }

    }
  }

  // POST request
  else
  {
    if ( (i_pos2 = s_req.find("HTTP")) != string::npos )
    {
      if (i_pos2 > 13)
      {
        s_ret = url_decode( s_req.substr(6,i_pos2-7) );

        //wrap::system_message(s_req);
        //wrap::system_message(string("data offset=") + tool::int2string(i_postpayloadoffset)); 
        i_pos = s_req.find("event=",i_postpayloadoffset );
        if(i_pos != string::npos)
        {
          get_request_parameters( url_decode( s_req.substr(i_pos) ), map_params);
        }
      }
    }

  }

#ifdef VERBOSE
  wrap::system_message( REQUEST + s_ret );
#endif

  if ( s_ret.empty() )
    s_ret = wrap::CONF->get_elem( "httpd.startsite" );
  else
    s_ret = remove_dots(s_ret);

  map_params["request"] = s_ret;

  return s_ret;
}

string
reqp::get_content_type(string &s_file)
{
  string s_ext(tool::get_extension( s_file ));

  if( s_ext == "" )
    s_ext = "default";

  return wrap::CONF->get_elem( "httpd.contenttypes." + s_ext );
}

void
reqp::parse_headers( string s_req, map<string,string> &map_params )
{
  int pos = s_req.find("\n");

  if (pos != string::npos)
  {
    map_params["QUERY_STRING"] = tool::trim(s_req.substr(0,pos-1));

    int pos2;
    do
    {
      string s_line( s_req.substr(0, pos) );
      pos2 = s_line.find(":");

      if (pos2 != string::npos && s_line.length() > pos2+1)
        map_params[ tool::trim(s_line.substr(0, pos2)) ] = tool::trim(s_line.substr(pos2+1));

      s_req = s_req.substr( s_line.size() + 1 );
      pos = s_req.find("\n");
    }
    while( pos != string::npos);
  } // if
}

int
reqp::htoi(string *p_str)
{
  int value, c;
  c = p_str->at(0);

  if( isupper(c) )
    c = tolower(c);

  value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

  c = p_str->at(1);

  if( isupper(c) )
    c = tolower(c);

  value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

  return value;
}

string
reqp::url_decode( string s_url )
{
  string s_dest = "";
  int i_len = s_url.size();
  int i_prv = i_len - 2;

  char c;
  for( int i = 0; i < i_len; ++i)
  {
    c = s_url.at(i);
    if( c == '+' )
    {
      s_dest += " ";
    }
    else if (c == '%' && i < i_prv)
    {
      string s_tmp = s_url.substr(i+1, 2);
      c = (char) htoi(&s_tmp);
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

string
reqp::get_from_header( string s_req, string s_hdr )
{
  size_t i_pos[2];
  if ( (i_pos[0] = s_req.find( s_hdr, 0 )) == string::npos )
    return "";

  if ( (i_pos[1] = s_req.find( "\n", i_pos[0]) ) == string::npos )
    return "";

  unsigned i_len = s_hdr.length();
  return s_req.substr( i_pos[0] + i_len, i_pos[1] - i_pos[0] - i_len - 1 );
}

string
reqp::parse( socketcontainer *p_sock, string s_req, map<string,string> &map_params, int &i_postpayloadoffset )
{

  // store all request informations in map_params. store the url in
  // map_params["request"].
  get_url( s_req, map_params, i_postpayloadoffset );

  parse_headers( s_req, map_params );
  string s_event( map_params["event"] );

  map_params["content-type"] = get_content_type( map_params["request"] );

  string s_rep( "" );


  if ( wrap::CONF->get_elem("httpd.enablecgi").compare("true") == 0 &&
       string::npos != map_params["request"].find(".cgi") )
  {
    s_rep.append( tool::shell_command(
                    wrap::CONF->get_elem("httpd.templatedir") + map_params["request"],
                    METH_RETSTRING ) );
  }
  else
  {
    // parse and get the requested html-template and also use
    // the values stored in map_params for %%KEY%% substituations.
    s_rep.append( wrap::HTML->parse( map_params ) );
  }

  // create the http header.

  string s_resp(s_http);
  if ( s_event.compare("stream") == 0 )
    s_resp.append( s_http_stream );

  s_resp.append( s_http_colength + tool::int2string(s_rep.size()) + "\r\n" +
                 s_http_cotype + map_params["content-type"] +
                 s_http_cotype_add + "\r\n" );

  s_resp.append(s_rep);


  // return the parsed html-template.
  return  s_resp;
}


string
reqp::remove_dots( string s_ret )
{
  // remove ".." from the request.
  size_t i_pos;

  if ( (i_pos = s_ret.find( ".." )) != string::npos )
    return remove_dots(s_ret.substr(0, i_pos));

  return s_ret;
}

#endif
