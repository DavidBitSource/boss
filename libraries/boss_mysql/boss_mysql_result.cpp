/* boss_mysql_result.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include "boss_mysql_result.h"
#include "boss_mysql_handler.h"

static const char F_Name1[] = "MySQL_Result::MySQL_Result(MySQL_Handler *, MYSQL_RES *)";
static const char F_Name2[] = "string MySQL_Result::operator () (const unsigned & , const unsigned &) const";
static const char F_Name3[] = "string MySQL_Result::operator () (const unsigned & , const string &) const";

MySQL_Result::MySQL_Result(MySQL_Handler * b, MYSQL_RES * r) throw(MySQL_QueryFailed) : base(b), result(r)
{
  base->push_res(this, result);
  MYSQL_FIELD * fields = mysql_fetch_field(result);
  if(!fields)
    throw MySQL_QueryFailed(F_Name1, mysql_error(base->handle));
  unsigned col = mysql_num_fields(result);
  for(unsigned i=0; i<col; i++)
    field_map[fields[i].name] = i;	
}

MySQL_Result::MySQL_Result(const MySQL_Result & r) : base(r.base), result(r.result), field_map(r.field_map)
{ if(result) base->push_res(this, result); }

MySQL_Result::~MySQL_Result()
{ if(result) base->pop_res(this, result); }

MySQL_Result & MySQL_Result::operator = (const MySQL_Result & r)
{
  if(base)
    base->pop_res(this, result);
  base      = r.base;
  result    = r.result;
  field_map = r.field_map;
  if(result) base->push_res(this, result);
  return *this;
}
	
string MySQL_Result::operator () (const unsigned & r, const unsigned & c) const throw(MySQL_QueryFailed, MySQL_IndexOutOfRange)
{
  if(r >= mysql_num_rows(result))
    throw MySQL_IndexOutOfRange(F_Name2, r);
  if(c >= mysql_num_fields(result))
    throw MySQL_IndexOutOfRange(F_Name2, c);
	
  mysql_data_seek(result, r);
  MYSQL_ROW res_row = mysql_fetch_row(result);
  if(res_row == 0)
    throw MySQL_QueryFailed(F_Name2, mysql_error(base->handle));
	
  if(res_row[c])
    return string(res_row[c]);
	
  return string();
}

string MySQL_Result::operator () (const unsigned & row, const string & col) const throw(MySQL_QueryFailed)
{
  MySQL_FieldMapCIt it = field_map.find(col);
  if(it == field_map.end())
    throw MySQL_QueryFailed(F_Name3, "no such column " + col);
		
  return operator () (row, (*it).second);
}

ostream & operator << (ostream & out, const MySQL_Result & r)
{
  unsigned col = r.col();
  unsigned row = r.row();
  for(unsigned i=0; i<row; i++) {
    for(unsigned j=0; j<col; j++)
      out << r(i, j) << " ";
    out << endl;
  }
  return out;
}
