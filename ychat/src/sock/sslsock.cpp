/*:*
 *: File: ./src/sock/sslsock.cpp
 *: 
 *: yChat; Homepage: ychat.buetow.org; Version 0.9.1-CURRENT
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

#include "../incl.h"

#ifdef OPENSSL
#ifndef SSLSOCK_CPP
#define SSLSOCK_CPP

#include "sslsock.h"

using namespace std;

sslsock::sslsock() : sock()
{
  s_certificate_path = wrap::CONF->get_elem( "httpd.ssl.certificatepath" );
  s_privatekey_path = wrap::CONF->get_elem( "httpd.ssl.privatekeypath" );
  p_ctx = NULL;
}

int
sslsock::_send(_socket *p_sock, const char *sz, int len)
{
  return SSL_write((SSL*)p_sock->p_ssl_context,sz, len);
}

int
sslsock::_read(_socket *p_sock,  char *sz, int len)
{
  return SSL_read((SSL*)p_sock->p_ssl_context,sz,len);
}

int
sslsock::_close(_socket *p_sock)
{
  SSL_free((SSL*)p_sock->p_ssl_context);
  sock::_close(p_sock);
}

int
sslsock::_make_server_socket(int i_port)
{
  SSL_METHOD *p_ssl_method;
  unsigned long e;
  char sz[1024];
  string s_error;

  int i_sock = sock::_make_server_socket(i_port);

  if (i_sock <= 0)
  {
    wrap::system_message(SSLERR1);
    return -1;
  }

  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();

  p_ssl_method = SSLv23_server_method();
  p_ctx = SSL_CTX_new (p_ssl_method);
  if (!p_ctx)
  {
    e = ERR_get_error();
    ERR_error_string_n(e, sz, sizeof(sz) - 1);
    s_error = sz;
    wrap::system_message(SSLERR1);
    return -1;
  }

  if (SSL_CTX_use_certificate_file(p_ctx, s_certificate_path.c_str(), SSL_FILETYPE_PEM) <= 0)
  {
    e = ERR_get_error();
    ERR_error_string_n(e, sz, sizeof(sz) - 1);
    s_error = sz;
    wrap::system_message(SSLERR1);
    return -1;
  }

  if (SSL_CTX_use_PrivateKey_file(p_ctx, s_privatekey_path.c_str(), SSL_FILETYPE_PEM) <= 0)
  {
    e = ERR_get_error();
    ERR_error_string_n(e, sz, sizeof(sz) - 1);
    s_error = sz;
    wrap::system_message(SSLERR1);
    return -1;
  }

  if (!SSL_CTX_check_private_key(p_ctx))
  {
    wrap::system_message(SSLERR2);
    return -1;
  }

  return i_sock;
}

bool
sslsock::_main_loop_do_ssl_stuff(int& i_new_sock)
{
  SSL* p_ssl = SSL_new(p_ctx);

  if (p_ssl == NULL || i_new_sock < 0)
  {
    wrap::system_message(SSLERR3);

    close(i_new_sock);
    if (p_ssl != NULL)
      SSL_free(p_ssl);

    return 1;
  }

  else
  {
    SSL_set_fd(p_ssl, i_new_sock);
    if (SSL_accept(p_ssl) == -1)
    {
      wrap::system_message(SSLERR4);
      close(i_new_sock);
      return 1;
    }

    map_certs[i_new_sock] = p_ssl;
  }

  return 0;
}

_socket*
sslsock::_create_container(int &i_sock)
{
  _socket* p_sock = sock::_create_container(i_sock);
  p_sock->p_ssl_context = map_certs[i_sock];
  return p_sock;
}

#endif
#endif
