/* boss_mysql_exceptions.h    $Revision: 1.3 $ $Date: 2007/04/16 09:43:33 $
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

#ifndef MYSQL_EXCEPTIONS_H
#define MYSQL_EXCEPTIONS_H

#include <string>
#include <iostream>

using namespace std;

/**
  @author Karlheinz Stoeber
*/

class MySQL_TwoStringException {
protected:	
	string f_name;
	string message;
public:
	MySQL_TwoStringException(const string & f_n, const string m) : f_name(f_n), message(m) {}
	virtual void printError(const bool & fatal = false) const = 0;
	virtual ~MySQL_TwoStringException() {}
};	

class MySQL_StringNumException {
protected:	
	string   f_name;
	unsigned message;
public:
	MySQL_StringNumException(const string & f_n, const unsigned m) : f_name(f_n), message(m) {}
	virtual void printError(const bool & fatal = false) const = 0;
	virtual ~MySQL_StringNumException() {}
};	

class MySQL_ConnectFailed : public MySQL_TwoStringException {
public:
	MySQL_ConnectFailed(const string & f_n, const string m) : MySQL_TwoStringException(f_n, m) {}
	void printError(const bool & fatal) const;
};

class MySQL_QueryFailed : public MySQL_TwoStringException {
public:
	MySQL_QueryFailed(const string & f_n, const string m) : MySQL_TwoStringException(f_n, m) {}
	void printError(const bool & fatal) const;
};

class MySQL_IndexOutOfRange : public MySQL_StringNumException {
public:
	MySQL_IndexOutOfRange(const string & f_n, const unsigned m) : MySQL_StringNumException(f_n, m) {}
	void printError(const bool & fatal) const;
};

#endif
