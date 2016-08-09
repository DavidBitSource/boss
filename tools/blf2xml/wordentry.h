/* wordentry.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#ifndef WORDENTRY_H
#define WORDENTRY_H

#include "syllableentry.h"

/**
 *@author Karlheinz Stoeber
 */

class c_WordEntry {
  string   key;
  string   real;
  string   sb_sym;
  bool     start;
  unsigned first;
  unsigned last;
  string   pros;
  c_SyllableEntry syllable;
  vector< c_SyllableEntry > syllables;
  bool isSyllableBoundarie(const string & p) const;
 public:
  c_WordEntry(const string & sb) : sb_sym(sb), start(true) {}
  void add(const c_LabelEntry & l);
  void finish(const c_LabelEntry & l);
  void setFirst(const unsigned & f) { first = f; }
  void setLast(const unsigned & l)  { last  = l; }
  void setPros(const string & p)    { pros  = p; }
  ostream & printXML(ostream & out, const string & ident) const;
};

#endif
