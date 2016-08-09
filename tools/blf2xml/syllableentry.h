/* syllableentry.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#ifndef SYLLABLEENTRY_H
#define SYLLABLEENTRY_H

#include "phoneentry.h"

/**
 *@author Karlheinz Stoeber
 */

class c_SyllableEntry {
	
  string   key;
  string   real;
  unsigned stress;
  string   sb_sym;
  unsigned first;
  unsigned last;
  vector< c_PhoneEntry > phones;

 public:
  friend ostream & operator << (ostream & out, const c_SyllableEntry & s);
  c_SyllableEntry() : stress(0) {}
  void add(const c_LabelEntry & l);
  void setFirst(const unsigned & f) { first = f; }
  void setLast(const unsigned & l)  { last  = l; }
  ostream & printXML(ostream & out, const string & indent) const;
};

inline ostream & operator << (ostream & out, const c_SyllableEntry & s)
{ return out << s.key << " " << s.real << endl; }

#endif
