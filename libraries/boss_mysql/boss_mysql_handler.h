/* boss_mysql_handler.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
   Copyright (C) 2000 University of Bonn.
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

/**
   @author Karlheinz Stoeber
*/

#ifndef MYSQL_HANDLER_H
#define MYSQL_HANDLER_H

#include <mysql.h>
#include <list>
#include <map>
#include <iterator>
#include "boss_mysql_exceptions.h"
#include "boss_mysql_result.h"

using namespace std;

typedef list< MySQL_Result * >               MySQL_ResultList;
typedef map< MYSQL_RES *, MySQL_ResultList > MySQL_ResultMap;
typedef MySQL_ResultMap::iterator            MySQL_ResultMapIt;
typedef MySQL_ResultMap::const_iterator      MySQL_ResultMapCIt;

/**
  *@author Karlheinz Stoeber
  */
class MySQL_Handler {

private:

  MYSQL           *handle;
  MySQL_ResultMap results;

  friend class MySQL_Result;

  void push_res(MySQL_Result * rclass, 	MYSQL_RES * rdata);
  void pop_res(MySQL_Result * rclass,   MYSQL_RES * rdata);
	
public:

  MySQL_Handler() throw(MySQL_ConnectFailed);
  MySQL_Handler(const string & user, const string & passwd, const string & database) throw(MySQL_ConnectFailed);
  MySQL_Handler(const string & host, const string & user, const string & passwd, const string & database) throw(MySQL_ConnectFailed);
  MySQL_Handler(const MySQL_Handler & h);
  ~MySQL_Handler();
  MySQL_Handler & operator = (const MySQL_Handler & h);

  MySQL_Result query(const char * query_string, const bool do_free=false);
  MySQL_Result query(const char * query_string, const unsigned & length, const bool do_free=false) throw(MySQL_QueryFailed);
	
  void print(ostream & out) const;
};

#endif
