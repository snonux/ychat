/*
 Notice: 
 
 All #defines which start with an CONFIG can be edited through
 gmake config in the main directory!
*/

#include "maps/hashmap.h"

// global variables.
#ifndef GLOB_H
#define GLOB_H

// Definition of boolean values.
#define true   1
#define false  0


/* - CONFIG -
 Should yhttpd get compiled with OpenSSL support?
*/
//#define OPENSSL

/* - CONFIG -
 Should yhttpd get compiled with comand line interface support?
*/
//#define CLI

/* - CONFIG -
 What should be the name of the config file?
*/
#define CONFILE "yhttpd.conf"

/* - DISABLED -
 Enable debugging options. 
*/
//#define DEBUG

/* - DISABLED -
 If you want to enable EXPERIMENTAL features, then set this val-
 ue to true. Else use false which is recommended! All experimen-
 al features are marked inside of the running yhttpd!
*/
//#define EXPERIM

/* - CONFIG -
 Should yhttpd get compiled with logging support? 
*/
#define LOGGING

/* - CONFIG -
 Please enter the highest networking port which is allowed to be 
 used. If yhttpd is unable to create the server socket on a cert-
 ain port, it will increment the port number and retries to cre-
 ate another socket on the incremented port number. This proced-
 ure will continue until MAXPORT has been reached.
*/
#define MAXPORT 65535

/* Specifies the max amount of lines to read from a HTTP request 
   header
*/
#define MAXLINES 30

/* Specifies the max length of a lines to read from a HTTP request 
   header
*/
#define MAXLENGTH 1024

/* - CONFIG -
 Should yhttpd get compiled with ncurses support?
*/
//#define NCURSES

/* - CONFIG -
 Please specify the maximum length of a HTTP post request. 
*/
#define POSTBUF 512

/* - CONFIG -
 Please specify the size of a temporary buffer. (Will be used f-
 or different tasks) 
*/
#define READBUF 2048

/* - CONFIG -
 Please specify the maximum length of a line read from a socket
 or a file. ( config-file, html-template )
*/
#define READSOCK 2048

/* - CONFIG -
 Upper bound (bytes) on how much unread request data sock::_close() will
 drain from a socket before giving up and closing anyway. Bounds the
 non-blocking drain loop against a client that keeps streaming data after
 we've half-closed the connection.
*/
#define DRAINMAX 8192

/* - CONFIG -
 In which prefix should yhttpd be installed if typing gmake inst-
 all? 
*/
#define PREFIX "/usr/local"

/* - CONFIG -
 DO NOT USE TOGETHER WITH NCURSES! Displays important server mes-
 ages. This one will print all messages to stdout if no NCURSES
 is defined. Don't use this until NCURSES is defined! all messag-
 es will appear in the ncurses interface anyways.
*/
//#define SERVMSG

/* - CONFIG -
 Set to true if you want yhttpd to catch the SIGSEGV signal. yhttpd
 will print a warning message into the system messages and will 
 not core dump if an error occurs. 
*/
//#define CTCSEGV

/* - CONFIG -
 Please chose if you want to use verbose server outputs or not. 
 The verbose messages will appear in the ncurses menu if ncurses
 is enabled or in the server-window if yhttpd has been compiled 
 without ncurses support. This option shows you all incoming
 requests with the client IP and port numbers. You probably want
 this to be turned off if you have heavy server load.
*/
//#define VERBOSE




// The following values define the positions of the data stats in the NCURSES interface.
#ifdef NCURSES
#define NCUR_SERVER_HEADER_X 21
#define NCUR_SERVER_HEADER_Y 2
#define NCUR_PORT_X 22
#define NCUR_PORT_Y 2
#define NCUR_HITS_X 23
#define NCUR_HITS_Y 2

#define NCUR_POOL_HEADER_X 21
#define NCUR_POOL_HEADER_Y 16
#define NCUR_POOL_WAIT_X 22
#define NCUR_POOL_WAIT_Y 16
#define NCUR_POOL_RUNNING_X 23
#define NCUR_POOL_RUNNING_Y 16

#define NCUR_DATA_HEADER_X 21
#define NCUR_DATA_HEADER_Y 35
#define NCUR_GARBAGE_X 22
#define NCUR_GARBAGE_Y 35
#define NCUR_CON_QUEUE_X 23
#define NCUR_CON_QUEUE_Y 35

#define NCUR_HTTPD_HEADER_X 21
#define NCUR_HTTPD_HEADER_Y 52
#define NCUR_NUM_ROOMS_X 22
#define NCUR_NUM_ROOMS_Y 52
#define NCUR_SESSION_X 23
#define NCUR_SESSION_Y 52

#define NCUR_CACHED_HEADER_X 21
#define NCUR_CACHED_HEADER_Y 68
#define NCUR_CACHED_DOCS_X 22
#define NCUR_CACHED_DOCS_Y 68
#define NCUR_CACHED_MODS_X 23
#define NCUR_CACHED_MODS_Y 68

#define NCUR_MENU_CHAR_X 0
#define NCUR_MENU_CHAR_Y 33
#define NCUR_UPTIME_X 0
#define NCUR_UPTIME_Y 44
#define NCUR_TIME_X 0
#define NCUR_TIME_Y 64

#endif

//////////////////////////////////////////////////////////////////////////////////////////
// DO NOT CHANGE ANYTHING BEHIND THIS LINE!
//////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

typedef int mod_func_t( void *v_arg );

struct container
{
  void* elem[4];
};

struct dynmod
{
  mod_func_t *the_func  ;
  void     *the_module;
};

typedef enum method_ {
  METH_NCURSES,
  METH_RETSTRING
} method;

// Define external executables:
#define GMAKE "/usr/local/bin/gmake \0"
#endif
