/* boss_mysql_handler.cpp    $Revision: 1.4 $ $Date: 2008/09/24 17:25:13 $
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

using namespace std;

#include "boss_mysql_handler.h"
#include "stdlib.h"
#include "string.h"

static const char F_Name1[] = "MySQL_Handler::MySQL_Handler()";
static const char F_Name2[] = "MySQL_Handler::MySQL_Handler(const string &, const string &, const string &)";
static const char F_Name3[] = "MySQL_Handler::MySQL_Handler(const string &, const string &, const string &)";
static const char F_Name4[] = "MySQL_Result MySQL_Handler::query(const char *, const unsigned &)";

MySQL_Handler::MySQL_Handler() throw(MySQL_ConnectFailed) : handle(mysql_init(0))
{
  if(!handle)
    throw MySQL_ConnectFailed(F_Name1, "init failed");
}

MySQL_Handler::MySQL_Handler(const string & user, const string & passwd, const string & database) throw(MySQL_ConnectFailed) :
  handle(mysql_init(0))
{
  if(!handle)
    throw MySQL_ConnectFailed(F_Name2, "init failed");
	
  MYSQL * tmp = mysql_real_connect(handle, 0, user.c_str(), passwd.c_str(), database.c_str(), 0, 0, 0);
  if(!tmp)
    throw MySQL_ConnectFailed(F_Name2, mysql_error(handle));

  handle = tmp;
}

MySQL_Handler::MySQL_Handler(const string & host, const string & user, const string & passwd, const string & database) throw(MySQL_ConnectFailed) :
  handle(mysql_init(0))
{
  if(!handle)
    throw MySQL_ConnectFailed(F_Name3, "init failed");
	
  MYSQL * tmp = mysql_real_connect(handle, host.c_str(), user.c_str(), passwd.c_str(), database.c_str(), 0, 0, 0);

  if(!tmp)
    throw MySQL_ConnectFailed(F_Name3, mysql_error(handle));

  handle = tmp;
}

MySQL_Handler::MySQL_Handler(const MySQL_Handler & h)
{
  cerr << "Copy operation currently not supported" << endl;
  exit(-1);
}

MySQL_Handler::~MySQL_Handler()
{
  MySQL_ResultMapIt it;
  for(it=results.begin(); it!=results.end(); it++)
    mysql_free_result((*it).first);
		
  mysql_close(handle);
}
 	
MySQL_Handler & MySQL_Handler::operator = (const MySQL_Handler & h)
{
  cerr << "Assignment operation currently not supported" << endl;
  exit(-1);
}

MySQL_Result MySQL_Handler::query(const char * query_string, const bool do_free)
{ return query(query_string, strlen(query_string), do_free); }

MySQL_Result MySQL_Handler::query(const char * query_string, const unsigned & length, const bool do_free) throw(MySQL_QueryFailed)
{	
  if(mysql_real_query(handle, query_string, length))
    throw MySQL_QueryFailed(F_Name4, mysql_error(handle));
	
  MYSQL_RES *res = mysql_store_result(handle);
  if(!res) {
    if(mysql_field_count(handle) != 0)
      throw MySQL_QueryFailed(F_Name4, mysql_error(handle));
    return MySQL_Result();
  }
  return MySQL_Result(this, res);
}	
	
void MySQL_Handler::push_res(MySQL_Result * rclass, MYSQL_RES * rdata)
{ 
  results[rdata].push_back(rclass); }

void MySQL_Handler::pop_res(MySQL_Result * rclass, MYSQL_RES * rdata)
{
  MySQL_ResultMapIt it = results.find(rdata);
  (*it).second.remove(rclass);
  if(!(*it).second.size()) {
    if(rdata)
      mysql_free_result(rdata);
    results.erase(it);
  }
}

void MySQL_Handler::print(ostream & out) const
{
  MySQL_ResultMapCIt it;
  for(it=results.begin(); it!=results.end(); it++) {
    out << (*it).first << " ";
    copy((*it).second.begin(), (*it).second.end(), ostream_iterator< MySQL_Result * >(out, " "));
    out << endl;
  }
}
