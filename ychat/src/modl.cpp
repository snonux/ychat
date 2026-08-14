/*:*
 *: File: ./src/modl.cpp
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

#ifndef MODL_CPP
#define MODL_CPP

#include <limits.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>

#include "modl.h"
#include "tool/dir.h"

using namespace std;

modl::modl()
{
  if ( wrap::CONF->get_elem( "httpd.modules.preloadcommands" ).compare( "true" ) == 0 )
    preload_modules( wrap::CONF->get_elem("httpd.modules.commandsdir") );

  if ( wrap::CONF->get_elem( "httpd.modules.preloadhtml" ).compare( "true" ) == 0 )
    preload_modules( wrap::CONF->get_elem("httpd.modules.htmldir") );
}

modl::~modl()
{
  // dlclose all the_module's first!
  run_func( &modl::dlclose_ );

  // then clean the hash map.
  unload_modules();
}

void
modl::preload_modules( string s_path )
{
  dir* p_dir = new dir();
  p_dir->open_dir( s_path );

  p_dir->read_dir();

  vector<string> dir_vec = p_dir->get_dir_vec();

  if ( ! dir_vec.empty() )
  {
    vector<string>::iterator iter = dir_vec.begin();

    do
    {
      if ( iter->length() >= 3 && iter->compare( iter->length()-3, 3, ".so" ) == 0 )
        cache_module( s_path + *iter, false );
    }
    while ( ++iter != dir_vec.end() );
  }

  dir_vec.clear();

  // This also closes the dir.
  delete p_dir;
}

void
modl::dlclose_( dynmod* mod )
{
  dlclose( mod->the_module );
  free   ( mod );
}

dynmod*
modl::cache_module( string s_name, bool b_print_sys_msg )
{
  void     *the_module = NULL;
  mod_func_t *the_func   = NULL;

  the_module = dlopen( s_name.c_str(), RTLD_LAZY );
  //the_module = dlopen( s_name.c_str(), RTLD_NOW );

  if ( the_module == NULL )
  {
    wrap::system_message( dlerror() );
    return NULL;
  }

  the_func = (mod_func_t*) dlsym( the_module, "extern_function" );

  if ( the_func == NULL )
  {
    wrap::system_message( dlerror() );
    return NULL;
  }

  if ( b_print_sys_msg )
    wrap::system_message( MODULEC + s_name.substr( s_name.find_last_of("/")+1 ) );

  dynmod *mod     = new dynmod; // encapsulates the function and module handler.
  mod->the_func   = the_func  ; // the function to execute
  mod->the_module = the_module; // the module handler to close if function

  // is not needed anymore.
  add_elem( mod, s_name  );

  // DO NOT CLOSE AS LONG THERE EXIST A POINTER TO THE FUNCTION
  // dlclose( module ); will be called in modl::~modl()!

  return mod;
}

dynmod*
modl::get_module( string s_name, string s_user )
{
  wrap::system_message( MODULER + s_name.substr( s_name.find_last_of("/")+1 ) + " (" + s_user + ")");
  dynmod* mod = get_elem( s_name );
  return ! mod ? cache_module( s_name, true ) : mod;
}

dynmod*
modl::get_module( string s_name )
{
  wrap::system_message( MODULER + s_name.substr( s_name.find_last_of("/")+1 ) );
  dynmod* mod = get_elem( s_name );
  return ! mod ? cache_module( s_name, true ) : mod;
}

void
modl::unload_modules()
{
  wrap::system_message( MODUNLO );

  // dlclose all the_module's first!
  run_func( &modl::dlclose_ );

  // then clean the hash map.
  shashmap<dynmod*>::clear();

}

void
modl::reload_modules()
{
  unload_modules();

  wrap::system_message( MODRELO );
  preload_modules( wrap::CONF->get_elem("httpd.modules.commandsdir") );
  preload_modules( wrap::CONF->get_elem("httpd.modules.htmldir") );
}

#endif
