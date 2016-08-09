/* wordentry.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include "wordentry.h"

vector< string > divideProsString(const string & p);

ostream & c_WordEntry::printXML(ostream & out, const string & indent) const
{
  vector< string > dpros(2);
  if(pros.size()) {
    dpros = divideProsString(pros);
  }
  out << indent     << "<WORD "
      << "TKey=\""  << key      << "\" "
      << "TReal=\"" << real     << "\" "
      << "PInt=\""  << dpros[0] << "\" "
      << "PMode=\"" << dpros[1] << "\" "
      << "First=\"" << first    << "\" "
      << "Last=\""  << last     << "\""
      << ">"        << endl;
  for(unsigned i=0; i<syllables.size(); i++)
    syllables[i].printXML(out, indent+"  ");
  out << indent << "</WORD>" << endl;
  return out;
}

bool c_WordEntry::isSyllableBoundarie(const string & p) const
{
  if(p.find_first_of(sb_sym) != string::npos && p[p.size()-1] != '-')
    return true;
  return false;
}

void c_WordEntry::add(const c_LabelEntry & l)
{
  key  += cleanPhon(getKan(l.phon));
  real += cleanPhon(getReal(l.phon));
  if(isSyllableBoundarie(l.phon)) {
    if(start) {
      start = false;
      syllable.add(l);
      syllable.setFirst(l.first);
    }
    else {
      syllable.setLast(l.first);
      syllables.push_back(syllable);
      syllable = c_SyllableEntry();
      syllable.add(l);
      syllable.setFirst(l.first);
    }
  }
  else {
    syllable.add(l);
  }	
}
	
void c_WordEntry::finish(const c_LabelEntry & l)
{		
  syllable.setLast(l.first);
  syllables.push_back(syllable);
  syllable = c_SyllableEntry();
}
