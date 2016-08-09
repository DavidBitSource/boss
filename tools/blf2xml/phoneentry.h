/* phoneentry.h    $Revision: 1.3 $ $Date: 2007/04/16 09:45:03 $
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

#ifndef PHONEMEENTRY_H
#define PHONEMEENTRY_H


#include<vector>
#include<string>
#include<iostream>

using namespace std;

/**
 *@author Karlheinz Stoeber
 */
struct c_LabelEntry {
  unsigned first;
  unsigned last;
  string   phon;
  string   pros;
};


/**
 *@author Karlheinz Stoeber
 */
class c_PhoneEntry {
  string   key;
  string   real;
  unsigned stress;
  unsigned first;
  unsigned last;
 public:
  c_PhoneEntry(const c_LabelEntry & l);
  ostream & printXML(ostream & out, const string & indent) const;
};

string cleanPhon(const string & p);
string getKan(const string & p);
string getReal(const string & p);

#endif
