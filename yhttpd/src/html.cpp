#ifndef HTML_CPP
#define HTML_CPP

#include <fstream>
#include "html.h"

using namespace std;

html::html( )
{
  set_name( wrap::CONF->get_elem( "httpd.templatedir" ) );
}

html::~html( )
{}

void
html::clear_cache( )
{
  clear();
  wrap::system_message( CLRHTML );

#ifdef NCURSES

  print_cached( 0 );
#endif
}

string
html::parse( map<string,string> &map_params )
{
  string s_file = map_params["request"];

  // check if s_file is in the container.
  string s_templ;

  // if not, read file.
  if ( ! shashmap<string>::exists( s_file ) )
  {
    string   s_path  = get_name();
    ifstream if_templ( s_path.append( s_file ).c_str(), ios::binary );

    if ( ! if_templ )
    {
      wrap::system_message( OFFFOUND + s_path );
      if(map_params["request"] == wrap::CONF->get_elem( "httpd.html.notfound" ))
        return "";

      map_params["request"] = wrap::CONF->get_elem( "httpd.html.notfound" );
      return parse( map_params );
    }

    char c_buf;
    while( !if_templ.eof() )
    {
      if_templ.get( c_buf );
      s_templ += c_buf;
    }

    if ( map_params["content-type"].compare(0,5,"text/") == 0 )
      s_templ.erase(s_templ.end()-1);

    if_templ.close();

    wrap::system_message( TECACHE + s_path );

    // cache file.
    shashmap<string>::add_elem(s_templ, s_file);
#ifdef NCURSES

    print_cached( shashmap<string>::size() );
#endif

  }
  else
  {
    s_templ = shashmap<string>::get_elem( s_file );
  }

  // find %%KEY%% token and substituate those.
  size_t pos[2];
  pos[0] = pos[1] = 0;

  for(;;)
  {
    pos[0] = s_templ.find( "%%", pos[1] );

    if ( pos[0] == string::npos )
      break;

    pos[0] += 2;
    pos[1]  = s_templ.find( "%%", pos[0] );

    if ( pos[0] == string::npos )
      break;

    // get key and val.
    string s_key = s_templ.substr( pos[0], pos[1]-pos[0] );
    string s_val = wrap::CONF->get_elem( s_key );

    // if s_val is empty use map_params.
    if ( s_val.empty() )
      s_val = map_params[ s_key ];

    // substituate key with val.
    s_templ.replace( pos[0]-2, pos[1]-pos[0]+4, s_val );

    // calculate the string displacement.
    int i_diff = s_val.length() - ( pos[1] - pos[0] + 4);

    pos[1] += 2 + i_diff;

  };

  return s_templ;
}


#ifdef NCURSES
void
html::print_cached( int i_docs )
{
  if ( !wrap::NCUR->is_ready() )
    return;

  mvprintw( NCUR_CACHED_DOCS_X, NCUR_CACHED_DOCS_Y, "Docs: %d ", i_docs);
  refresh();
}

#endif

#endif

