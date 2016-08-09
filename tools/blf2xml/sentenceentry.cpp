/* sentenceentry.cpp    $Revision: 1.3 $ $Date: 2006/06/23 10:15:49 $
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

#include "sentenceentry.h"
#include <sstream>

vector< string > divideProsString(const string & p)
{
  vector< string > res;
  string tmp(p), field;
  for(unsigned i=0; i<p.size(); i++) {
    if(tmp[i] == ',')
      tmp[i] = ' ';
  }
  stringstream ins(tmp.c_str());
  while(ins>>field)
    res.push_back(field);
  return res;
}

char c_SentenceEntry::isSentenceDelimiter(const string & p) const
{
  vector< string > pros_inf = divideProsString(p);
  if(pros_inf.size() > 1 && pros_inf[0] == "5")
    return pros_inf[1][0];
  return 0;		
}

ostream & c_SentenceEntry::printXML(ostream & out, const string & ident)
{
  if(type == 0) {
    cerr << "Warning: " << file << " no sentence type given. Assuming \"statement\"!" << endl;
    if(words.size())
      words.back().setPros("5,.");
    out << ident << "<SENTENCE "
	<< "File=\"" << file << "\" "
	<< "Type=\"" << "." << "\""
	<< ">" << endl;
  }
  else {
    out << ident << "<SENTENCE "
	<< "File=\"" << file << "\" "
	<< "Type=\"" << type << "\""
	<< ">" << endl;
  }
  for(unsigned i=0; i<words.size(); i++)
    words[i].printXML(out, ident+"  ");
  out << ident << "</SENTENCE>" << endl;
  return out;
}

bool c_SentenceEntry::isWordBoundary(const string & p) const
{
  if(p.find(wb_sym) != string::npos)
    return true;
  return false;
}

void c_SentenceEntry::add(const c_LabelEntry & l)
{
  if (l.phon[l.phon.size()-1] != '-' || (l.phon[l.phon.size()-2] == '#' && l.phon[l.phon.size()-1] == '-')) {
    if(isWordBoundary(l.phon)) {
      if(l.pros.size())
	type = isSentenceDelimiter(l.pros);
      if(start) {
	word.add(l);
	word.setPros(l.pros);
      word.setFirst(l.first);
      start = false;
    }
      else {
	word.setLast(l.first);
	word.finish(l);
	words.push_back(word);
	word = c_WordEntry(sb_sym);
	word.setPros(l.pros);
	word.setFirst(l.first);
	word.add(l);
      }
    }
    else {
      word.add(l);
    }	
  }
}
	
