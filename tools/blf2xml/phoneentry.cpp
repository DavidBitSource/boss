/* phoneentry.cpp    $Revision: 1.3 $ $Date: 2010/05/18 15:00:00 $
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

#include "phoneentry.h"

string cleanPhon(const string & p)
{
  string tmp(p);
  size_t j=0;
  for(size_t i=0; i<p.size(); i++) {
    switch(p[i]) {
    case '#':
    case '+':
      //			case '_': // Removed, to account for diacritics in TReal
    case '.':
    case '%':
    case '"':
      //			case '\'': // Removed to allow for S\ in TReals and \ as diacritic
      break;
    default:
      tmp[j++] = p[i];
    }
  }
  tmp.resize(j);
  return tmp;
}

string getKan(const string & p) {
  string tmp(p);
  size_t pos;
  if((pos = p.find('+')) != string::npos)
    return string();
  if((pos = p.find('-')) != string::npos)
    tmp = string(p, 0, pos);
  return tmp;
}

string getKanAndInsertions(const string & p) {
  string tmp(p);
  size_t pos;
  if((pos = p.find('-')) != string::npos)
    tmp = string(p, 0, pos);
  return tmp;
}

string getReal(const string & p) {
  string tmp(p);
  size_t pos;
  if((pos = p.find('-')) != string::npos)
    tmp = string(p, pos+1);
  return tmp;
}

ostream & c_PhoneEntry::printXML(ostream & out, const string & indent) const
{
  out << indent << "<PHONE "
      << "TKey=\""   << key    << "\" "
      << "TReal=\""  << real   << "\" "
      << "Stress=\"" << stress << "\" "
      << "First=\""  << first  << "\" "
      << "Last=\""   << last   << "\""
      << ">"
      << "</PHONE>" << endl;
  return out;
}

c_PhoneEntry::c_PhoneEntry(const c_LabelEntry & l) :
  key(cleanPhon(getKanAndInsertions(l.phon))), real(cleanPhon(getReal(l.phon))), stress(0), first(l.first), last(l.last)
{
  string tmp=getReal(l.phon);
  if(tmp.find('%')!=string::npos) {
    stress = 2;
  }
  if(tmp.find('"')!=string::npos) {
    stress = 1;
  }
}
