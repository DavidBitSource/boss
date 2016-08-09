/* boss_cscomm.h    $Revision: 1.6 $ $Date: 2007/04/16 09:43:33 $
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

#ifndef __BOSSCSCOMMH__
#define __BOSSCSCOMMH__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <inttypes.h>

#include <string>
#include <fstream>

using namespace std;

#define DEF_PORT_ID 27673

typedef enum {SERVER, CLIENT} ConnectionType;

struct Signal {
  string id;
  uint32_t size;
  short * signal;
};

struct Document {
  string id;
  string doc;
};

/** A class for socket-based communication between BOSS server and client
    @author Karlheinz Stoeber
    @author Harald Ketzer
    @author Stefan Breuer
*/

class BOSSCSComm {
 private:
  int                server_socket,boss_socket;
  struct sockaddr_in serv_addr;
  socklen_t          sock_len;
  ConnectionType     type;
  uint32_t readSize();
  char * readBytes(uint32_t & size);
  void writeSize(uint32_t size);
 public:
  BOSSCSComm(const string & host_name, const ConnectionType & T, const short  port_id);
  ~BOSSCSComm();
  void     sendDocument(const string & id, const string & s);
  void     sendSignal(const string & id, const char * d, const uint32_t & s);
  Document receiveDocument();
  Signal   receiveSignal();
  string   acceptConnection();
  void     closeSock();
  void     closeServer();
};

#endif
