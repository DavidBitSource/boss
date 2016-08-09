/* sentenceentry.h    $Revision: 1.4 $ $Date: 2006/06/23 10:15:49 $
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

#ifndef SENTENCEENTRY_H
#define SENTENCEENTRY_H

#include "wordentry.h"

/**
 *@author Karlheinz Stoeber
 */

class c_SentenceEntry {
  c_WordEntry word;
  vector< c_WordEntry > words;
  string wb_sym;
  string sb_sym;
  bool   start;
  string file;
  char   type;
  bool isWordBoundary(const string & p) const;
  char isSentenceDelimiter(const string & p) const;
 public:
  c_SentenceEntry(const string & f, const string & wb, const string & sb) :
    word(sb), wb_sym(wb), sb_sym(sb), start(true), file(f), type(0) {}
  void add(const c_LabelEntry & l);
  ostream & printXML(ostream & out, const string & ident);
};

#endif
