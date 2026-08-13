/*:*
 *: File: ./src/cli/cli.cpp
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

#ifndef CLI_CPP
#define CLI_CPP

#include "cli.h"

#ifdef CLI

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

using namespace std;

cli::cli( )
{}

cli::~cli()
{}

int
cli::parse_input( string s_input )
{
  string s_param = "";
  unsigned long ul_pos;

  // Check for empty string or ignore leading whitespaces
  while (1)
  {
    if (s_input.empty())
    {
#ifndef READLINE
      cout << CLIPRMI;
#endif

      return 1;
    }

    ul_pos = s_input.find_first_of(" ");

    if (ul_pos != 0)
      break;
    s_input.erase(s_input.begin());
  }

  if ( ul_pos != (unsigned long) string::npos )
  {
    s_param = s_input.substr(ul_pos+1);
    s_input = s_input.substr(0, ul_pos);
  }

  if ( s_input.compare("help") == 0 || s_input.compare("h") == 0)
  {
    cout << CLIPRMO << "COMMAND LINE INTERFACE HELP MENU" << endl
    << CLIPRMO << " !command      - Uses system to run a command" << endl;
#ifdef DEBUG

    cout << CLIPRMO << " (d)ebug       - Starts debug routine (cli.cpp)" << endl;
#endif

    cout << CLIPRMO << " (du)mp [part] - Prints out a dump of the data structure" << endl;
    cout << CLIPRMO << "		    (Run without part to list all possibilities)" << endl;
    cout << CLIPRMO << " (e)cho VAR    - Prints out configuration value of VAR" << endl;
    cout << CLIPRMO << "                 Wildcards can be used too, example: echo http*" << endl;
    cout << CLIPRMO << " (h)elp        - Prints out this help!" << endl;
    //<<*
    cout << CLIPRMO << " (m)ysql       - Runs MySQL client on yChat DB" << endl
    << CLIPRMO << " (rel)oad      - Reloads all modules" << endl;
    //*>>

#ifdef EXPERIM

    cout << CLIPRMO << " (re)conf      - Reloads configuration (EXPERIMENTAL)" << endl;
#endif

    cout << CLIPRMO << " (r)usage      - Shows current resource usage" << endl
    << CLIPRMO << " (ru)sageh     - Shows resource usage history (yChat needs to run > 1 day)" << endl
    << CLIPRMO << " (set) VAR VAL - Sets configuration value VAR to VAL" << endl
    << CLIPRMO << " (sh)ell       - Runs a system shell" << endl
    << CLIPRMO << " (shut)down    - Shuts down the whole server" << endl
    << CLIPRMO << " (t)ime        - Prints out time and uptime" << endl;
    cout << CLIPRMO << " (unl)oad      - Unloads all loaded modules" << endl;//<<
    cout << CLIPRMO << " (u)nset VAR   - Deletes configuration value VAR" << endl
    << CLIPRMO << " (v)ersion     - Prints out version" << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  else if ( s_input.at(0) == '!' )
  {
    system( s_input.substr(1).c_str() );
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }

#ifdef DEBUG
  else if ( s_input.compare("d") == 0 || s_input.compare("debug") == 0 )
  {
    debug_routine();
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
#endif
  else if ( s_input.compare("du") == 0 || s_input.compare("dump") == 0 )
  {
    dump d(vectorize(s_param));
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  else if ( s_input.compare("echo") == 0 || s_input.compare("e") == 0 )
  {
    string s_val;

    // Check wildcards
    unsigned long ul_pos = s_param.find("*");
    if ( ul_pos != (unsigned long) string::npos )
    {
      s_param = s_param.substr( 0, ul_pos );
      vector<string>* p_vec = wrap::CONF->get_key_vector();
      sort(p_vec->begin(), p_vec->end());
      vector<string>::iterator iter;

      for ( iter = p_vec->begin(); iter != p_vec->end(); iter++ )
        if ( iter->find(s_param) == 0 )
          s_val.append( *iter + " := " +  wrap::CONF->get_elem(*iter) + "\n" + CLIPRMO );
      delete p_vec;
    }
    else
    {
      s_val = wrap::CONF->get_elem(s_param);
    }

    if ( s_val.empty() )
      s_val = "Value not set";

    cout << CLIPRMO << s_val << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }

  //<<*
  else if ( s_input.compare("mysql") == 0 || s_input.compare("m") == 0 )
  {
    cout << CLIPRMO << CLIMSQL << endl;

    system((wrap::CONF->get_elem("chat.system.mysqlclient") + " -p -h " +
            wrap::CONF->get_elem("chat.database.serverhost") + " -u " +
            wrap::CONF->get_elem("chat.database.user") ).c_str());

    cout << CLIPRMO << CLIWELC << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  //*>>


  //<<*
  else if ( s_input.compare("reload") == 0 || s_input.compare("rel") == 0 )
  {
    cout << CLIPRMO;
    wrap::MODL->reload_modules();
    cout << MODRELO << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  //*>>

#ifdef EXPERIM
  else if ( s_input.compare("reconf") == 0 || s_input.compare("re") == 0 )
  {
    wrap::CHAT->reconf();
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
#endif

  else if ( s_input.compare("rusage") == 0 || s_input.compare("r") == 0 )
  {
    print_rusage();
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  else if ( s_input.compare("ru") == 0 || s_input.compare("rusageh") == 0 )
  {
    cout << wrap::STAT->get_rusage_history( "ru_maxrss", string(CLIPRMO) + " " );
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  else if ( s_input.compare("set") == 0 )
  {
    string s_varname = "";
    ul_pos = s_param.find_first_of(" ");

    if ( ul_pos != string::npos )
    {
      s_varname = s_param.substr(0, ul_pos);

      if ( s_param.length() > ul_pos+1 )
        s_param = s_param.substr(ul_pos+1);

      else
        s_param = "";
    }

    string s_old_val = wrap::CONF->get_elem(s_varname);

    if ( !s_old_val.empty() )
    {
      cout << CLIPRMO << "Old value: " << s_old_val << endl;
      wrap::CONF->del_elem(s_varname);
    }

    wrap::CONF->add_elem(s_param, s_varname);
    cout << CLIPRMO << "New value: " << s_param << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  else if ( s_input.compare("shell") == 0 || s_input.compare("sh") == 0 )
  {
    cout << CLIPRMO << CLISHEL << endl;
    system(wrap::CONF->get_elem("httpd.system.shell").c_str());
    cout << CLIPRMO << CLIWELC << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  else if ( s_input.compare("shutdown") == 0 || s_input.compare("shut") == 0 )
  {
    exit(0);
  }
  else if ( s_input.compare("t") == 0 || s_input.compare("time") == 0 )
  {
    cout << CLIPRMO  << "Time:   " << wrap::TIMR->get_time() << endl
    << CLIPRMO  << "Uptime: " << wrap::TIMR->get_uptime() << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }

  //<<*
  else if ( s_input.compare("unl") == 0 || s_input.compare("unload") == 0 )
  {
    cout << CLIPRMO;
    wrap::MODL->unload_modules();
    cout << MODUNLO << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  //*>>

  else if ( s_input.compare("u") == 0 || s_input.compare("unset") == 0 )
  {
    if (!s_param.empty())
    {
      string s_old_val = wrap::CONF->get_elem(s_param);
      if ( !s_old_val.empty() )
      {
        cout << CLIPRMO << "Deleted variable " << s_param << " with value: " << s_old_val << endl;
        wrap::CONF->del_elem(s_param);
      }
      else
      {
        cout << CLIPRMO << "Variable " << s_param << " was not set" << endl;
      }
#ifndef READLINE
      cout << CLIPRMI;
#endif

    }
  }
  else if ( s_input.compare("v") == 0 || s_input.compare("version") == 0 )
  {
    cout << CLIPRMO  << tool::ychat_version() << " " << UNAME << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }
  else
  {
    cout << CLIPRMO << CLIHELP << "\"" << s_input << "\"" << endl;
#ifndef READLINE

    cout << CLIPRMI;
#endif

  }

  return 1;
}

void
cli::start()
{
  start(NULL);
}

void
cli::start(void* p_void)
{
  cout << CLIPRMO << CLIWELC << endl;

  string s_input;

#ifndef READLINE

  cout << CLIPRMI;
#endif

  while (1)
  {
#ifndef READLINE
    getline(cin, s_input);
#else

    char *c_line = readline(CLIPRMI);
    s_input = string(c_line);
    free(c_line);
#endif

    if (!parse_input(s_input))
      break;
  }
}

void
cli::print_rusage()
{
  rusage* p_rusage = new rusage;
  getrusage( RUSAGE_SELF, p_rusage );

  cout << CLIPRMO << "ru_maxrss:   " << p_rusage->ru_maxrss   << "\t(max resident set size)" << endl
  << CLIPRMO << "ru_ixrss:    " << p_rusage->ru_ixrss    << "\t(integral shared text memory size)" << endl
  << CLIPRMO << "ru_idrss:    " << p_rusage->ru_idrss    << "\t(integral unshared data size)" << endl
  << CLIPRMO << "ru_isrss:    " << p_rusage->ru_isrss    << "\t(integral unshared stack size)" << endl
  << CLIPRMO << "ru_minflt:   " << p_rusage->ru_minflt   << "\t(page reclaims)" << endl
  << CLIPRMO << "ru_majflt:   " << p_rusage->ru_majflt   << "\t(page faults)" << endl
  << CLIPRMO << "ru_nswap:    " << p_rusage->ru_nswap    << "\t(swaps)" << endl
  << CLIPRMO << "ru_inblock:  " << p_rusage->ru_inblock  << "\t(block input operations)" << endl
  << CLIPRMO << "ru_oublock:  " << p_rusage->ru_oublock  << "\t(block output operations)" << endl
  << CLIPRMO << "ru_msgsnd:   " << p_rusage->ru_msgsnd   << "\t(messages sent)" << endl
  << CLIPRMO << "ru_msgrcv:   " << p_rusage->ru_msgrcv   << "\t(messages received)" << endl
  << CLIPRMO << "ru_nsignals: " << p_rusage->ru_nsignals << "\t(signals received)" << endl
  << CLIPRMO << "ru_nvcsw:    " << p_rusage->ru_nvcsw    << "\t(voluntary context switches)" << endl
  << CLIPRMO << "ru_nivcsw:   " << p_rusage->ru_nivcsw   << "\t(involuntary context switches)" << endl;

  delete p_rusage;
}

vector<string>
cli::vectorize(string s_param)
{
  vector<string> vec_ret;
  unsigned long ul_pos;

  for (ul_pos = s_param.find(" ");
       ul_pos != (unsigned long) string::npos;
       ul_pos = s_param.find(" "))
  {
    vec_ret.push_back(s_param.substr(0, ul_pos));
    s_param = s_param.substr(ul_pos+1);
  }

  if (!s_param.empty())
    vec_ret.push_back(s_param);

  return vec_ret;
}

#ifdef DEBUG
void
cli::debug_routine()
{
  rusage* p_rusage = new rusage;
  for (;;)
  {
    /*
      ossl *p_tmp = new ossl;
      getrusage( RUSAGE_SELF, p_rusage );
       cout << CLIPRMO << "ru_maxrss:   " << p_rusage->ru_maxrss   << "\t(max resident set size)" << endl;
      delete p_tmp;
     */
  }
  delete p_rusage;
}
#endif

#endif
#endif
