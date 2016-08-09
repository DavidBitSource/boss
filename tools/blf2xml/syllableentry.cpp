/* syllableentry.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include "syllableentry.h"

ostream & c_SyllableEntry::printXML(ostream & out, const string & indent) const
{
  out << indent
      << "<SYLLABLE "
      << "TKey=\""    << key    << "\" "
      << "TReal=\""   << real   << "\" "
      << "Stress=\"" << stress << "\" "
      << "First=\""  << first  << "\" "
      << "Last=\""   << last   << "\""
      << ">" << endl;
  for(unsigned i=0; i<phones.size(); i++)
    phones[i].printXML(out, indent+"  ");
  out << indent << "</SYLLABLE>" << endl;
  return out;
}

void c_SyllableEntry::add(const c_LabelEntry & l)
{
  string tmp = getReal(l.phon);
  if(tmp.find('%')!=string::npos) {
    stress = 2;
  }
  if(tmp.find('"')!=string::npos) {
    stress = 1;
  }
	
  key  += cleanPhon(getKan(l.phon));
  tmp   = cleanPhon(getReal(l.phon));
  real += tmp;
  if(tmp.size())
    phones.push_back(c_PhoneEntry(l));
}
