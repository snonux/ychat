/*:*
 *: File: ./src/sock/sslsock.h
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

#include "../incl.h"

#ifdef OPENSSL
#ifndef SSLSOCK_H
#define SSLSOCK_H

#include "sock.h"

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

class sslsock : public sock
{
private:
  SSL_CTX* p_ctx;
  string s_certificate_path;
  string s_privatekey_path;
  map<int,void*> map_certs;

public:

  sslsock( );
  ~sslsock( );

  int _send(_socket *p_sock, const char *sz, int len);
  int _read(_socket *p_sock, char *sz, int len);
  int _close(_socket *p_sock);
  bool _main_loop_do_ssl_stuff(int &i_new_sock);
  _socket* _create_container(int& i_sock);
  int _make_server_socket(int i_port);
};

#endif
#endif
