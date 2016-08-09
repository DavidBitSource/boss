/* boss_cscomm.h    $Revision: 1.6 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2001 University of Bonn.
   http://www.ikp.uni-bonn.de/boss

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/** A class for socket-based communication between BOSS server and client
    @author Karlheinz Stoeber
    @author Harald Ketzer
    @author Stefan Breuer
*/

#include "boss_cscomm.h"
#include <stdlib.h>
#include <string.h>


BOSSCSComm::BOSSCSComm(const string & host_name, const ConnectionType & T, const short  port_id = DEF_PORT_ID) : type(T)
{
  struct protoent * p = getprotobyname("tcp");
  if (type==SERVER){
    server_socket = socket(PF_INET, SOCK_STREAM, p->p_proto);
    if(boss_socket == -1) {
      perror("Socket creation failed:");
      exit(EXIT_FAILURE);
    }
  }
  else {
    boss_socket = socket(PF_INET, SOCK_STREAM, p->p_proto);
    if(boss_socket == -1) {
      perror("Socket creation failed:");
      exit(EXIT_FAILURE);
    }
  }

  struct hostent * h = gethostbyname(host_name.c_str());

  /*** if the dns name of "host_name" can't be resolved, use localhost as default servername.
	else boss_server dies with an error*/
  if (h == NULL) {
	  h = gethostbyname("localhost");
  }

  memcpy(&(serv_addr.sin_addr.s_addr), h->h_addr, h->h_length);
  serv_addr.sin_port   = htons(port_id);
  sock_len = sizeof(serv_addr);
  if(type == SERVER) {
    serv_addr.sin_family = PF_INET;
    if(bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr))== -1) {
      perror("Bind failed");
      exit(EXIT_FAILURE);
    }
    if(getsockname(server_socket, (struct sockaddr *)&serv_addr, &sock_len) == -1) {
      perror("Bind failed");
      exit(EXIT_FAILURE);
    }
    if(listen(server_socket, 3) == -1) {
      perror("Listen failed");
      exit(EXIT_FAILURE);
    }
  }
  else {
    serv_addr.sin_family = PF_INET;
    if(connect(boss_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
      perror("Connection failed");
      exit(EXIT_FAILURE);
    }
  }
}

BOSSCSComm::~BOSSCSComm()
{
  endprotoent();
  endhostent();
  if (type==SERVER){
    closeServer();
  }
  closeSock();
}

string BOSSCSComm::acceptConnection()
{
  if (type==SERVER){
    if((boss_socket = accept(server_socket, (struct sockaddr *)&serv_addr, &sock_len)) == -1 ) {
      perror("Accept failed");
      exit(EXIT_FAILURE);
    }
    return string(inet_ntoa(serv_addr.sin_addr));
  }
  return "";
}

void BOSSCSComm::sendDocument(const string & id, const string & s)
{
  //Determine length of id and length of string to send,
  //then transmit in the following order:
  //size of id, id, size of string, string (document).
  uint32_t id_size = id.length();
  uint32_t string_size = s.length();
  writeSize(id_size);
  write(boss_socket, (const void *)id.c_str(), id_size);
  writeSize(string_size);
  write(boss_socket, (const void *)s.c_str(), string_size);
}

void BOSSCSComm::sendSignal(const string & id, const char * d, const uint32_t & s)
{
  //Determine length of id and length of signal to send,
  //then transmit in the following order:
  //size of id, id, size of signal, signal.
  uint32_t id_size = id.length();
  uint32_t n_id_size = htonl(id_size);
  write(boss_socket, (const void *)&n_id_size, sizeof(uint32_t));
  write(boss_socket, (const void *)id.c_str(), id_size);
  uint32_t signal_size = s;
  uint32_t n_signal_size = htonl(s);
  write(boss_socket, (const void *)&n_signal_size, sizeof(uint32_t));
  write(boss_socket, (void *) d, signal_size);
}

Document BOSSCSComm::receiveDocument()
{
  //Receive a document from the client.
  //First, receive the size of the id string, then the id itself.
  //Afterwards, read the size of the document from the stream, then the document.
  //All size parameters for strings are assumed to be the sizes without trailing '\0'.
  //Thus, all char [] for receiving strings are size+1 - '\0' is inserted after reception.
  //Some read statements are encapsulated by while loops to prevent read operations from stopping
  // at '\0' characters.
  uint32_t id_size = readSize();
  char * id_buffer = readBytes(id_size);
  uint32_t string_size = readSize();
  char * string_buffer = readBytes(string_size);
  Document doc;
  doc.id = string(id_buffer);
  doc.doc = string(string_buffer);
  delete [] id_buffer;
  delete [] string_buffer;
  return doc;
}

Signal BOSSCSComm::receiveSignal()
{
  //Receive a speech signal from the client.
  //First, receive the size of the id string, then the id itself.
  //Afterwards, read the size of the signal from the stream, then read the signal.
  //All size parameters for strings and signals are assumed to be the sizes without trailing '\0'.
  //Thus, all char [] for receiving strings are size+1 - '\0' is inserted after reception.
  //Some read statements are encapsulated by while loops to prevent read operations from stopping
  // at '\0' characters.
  uint32_t id_size = readSize();
  char * id_buffer = readBytes(id_size);
  uint32_t signal_size = readSize();
  char * signal_buffer = readBytes(signal_size);
  Signal sig;
  sig.id = string(id_buffer);
  sig.size = signal_size;
  sig.signal = (short *)signal_buffer;
  delete [] id_buffer;
  // signal buffer cannot be deleted yet. A destructor has to be added, if boss_cscomm is to be
  //  used multiple times,
  return sig;
}

void BOSSCSComm::closeSock()
{ if((close (boss_socket))== -1){
  	perror("Closing socket failed");
	exit(EXIT_FAILURE);
        }
}

void BOSSCSComm::closeServer()
{ if((close (server_socket))== -1){
	perror("Closing socket failed");
	exit(EXIT_FAILURE);
  }
}

uint32_t BOSSCSComm::readSize() {
  char * buf = new char[sizeof(uint32_t)];
  uint32_t value(0);
  uint32_t shift=0;
  for (uint32_t i=0; i<sizeof(uint32_t); i++) {
    read(boss_socket, (void *)&buf[i], 1);
    value += uint32_t(buf[i]) << shift;
    shift += 8;
  }
  delete [] buf;
  return ntohl(value);
}

char * BOSSCSComm::readBytes(uint32_t & size) {
  char * buf = new char[size+1];
  uint32_t i(0);
  while ((i += read(boss_socket, (void *)&buf[i], size-i)) < size) {}
  buf[size]='\0';
  return buf;
}

void BOSSCSComm::writeSize(uint32_t size) {
  size = htonl(size);
  char * buf = (char *)&size;
  for (uint32_t i=0; i<sizeof(uint32_t); i++)
    write(boss_socket, (void *)&buf[i], 1);
}
