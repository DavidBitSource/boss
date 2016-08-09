/* boss_mysql_exceptions.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include "boss_mysql_exceptions.h"
#include "stdlib.h"

void MySQL_ConnectFailed::printError(const bool & fatal = false) const
{
	if(fatal) {
		cerr << f_name << " : Fatal: " << message << endl; exit(-1);
  }
	else {
		cerr << f_name << " : Warning: " << message << ". Ignoring failure!" << endl;
  }
}

void MySQL_QueryFailed::printError(const bool & fatal = false) const
{
	if(fatal) {
		cerr << f_name << " : Fatal: " << message << endl; exit(-1);
  }
	else {
		cerr << f_name << " : Warning: " << message << ". Ignoring failure!" << endl;
  }
}

void MySQL_IndexOutOfRange::printError(const bool & fatal = false) const
{
	if(fatal) {
		cerr << f_name << " : Fatal: Index out of range: " << message << endl; exit(-1);
  }
	else {
		cerr << f_name << " : Warning: Index out of range: " << message << ". Ignoring failure!" << endl;
  }
}
