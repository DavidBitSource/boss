/* boss_mysql_result.h    $Revision: 1.3 $ $Date: 2007/04/16 09:43:33 $
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

#ifndef MYSQL_RESULT_H
#define MYSQL_RESULT_H

#include <mysql.h>
#include "boss_mysql_exceptions.h"
#include <map>

using namespace std;

typedef map< string, unsigned >        MySQL_FieldMap;
typedef MySQL_FieldMap::iterator       MySQL_FieldMapIt;
typedef MySQL_FieldMap::const_iterator MySQL_FieldMapCIt;

// Forward declaration
class MySQL_Handler;

/**
  *@author Karlheinz Stoeber
  * This class handles a result set which was returned by the mysql_real_query call.
  * To avoid copy operations of the whole data returned by the query, this class uses
  * only pointers to the data set. The data management is implemented in the handler
  * class. Therefore, a map is used which stores a pointer to the data as key and a list
  * of pointers to instances of this class as item. As long as instances of this
  * class are connected to the data, the data memory will not be free.
  * As a consequence, this class gives only read-only access to the result set.
*/
class MySQL_Result {
	
  MySQL_Handler   *base;
  MYSQL_RES       *result;
  MySQL_FieldMap  field_map;

 public:
	
  friend ostream & operator << (ostream & out, const MySQL_Result & r);
	
  MySQL_Result() : base(0), result(0) {}
  MySQL_Result(MySQL_Handler * b, MYSQL_RES * r) throw(MySQL_QueryFailed);
  MySQL_Result(const MySQL_Result & r);
  ~MySQL_Result();
  MySQL_Result & operator = (const MySQL_Result & r);

  string operator () (const unsigned & row, const unsigned & col) const throw(MySQL_QueryFailed, MySQL_IndexOutOfRange);
  string operator () (const unsigned & row, const string & col) const throw(MySQL_QueryFailed);
  unsigned col() const { return mysql_num_fields(result); }
  unsigned row() const { return mysql_num_rows(result); }
  bool valid() const { return result; }
};

#endif
