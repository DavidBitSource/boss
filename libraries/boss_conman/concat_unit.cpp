/* concat_unit.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2003 University of Bonn.
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/**
 * @file concat_unit.cpp
   @author Stefan Breuer
*/

/**
 * @addtogroup boss_concat
 * @{
 */

#include "concat_unit.h"

CM_Unit::CM_Unit(BOSS::Config & config, MySQL_Handler * db, xercesc::DOMNode * n,
		 const string & t, const string & p, const string & i, const string & s)
  : ConmanUnit(config, db, n, t, i)
{
  first = start_index;
  last = end_index;
  cerr << endl
       << "::: CM_Unit:       " << tkey << endl
       << "::: first:         " << first << " " << (double(first)/16000) << endl
       << "::: last:          " << last  << " " <<  (double(last)/16000)  << endl
       << "::: length:        " << last-first << " " <<  (double(last-first)/16000) << endl;
//  string file_name = getSentenceFileName(i);
  loadFromFile(BOSS::makeFilePath(p,file_name+s), first, last, LEFTRIGHTOL, LEFTRIGHTOL);
  start = LEFTRIGHTOL;
  end = signal.size()-LEFTRIGHTOL;
  cerr  << "::: start:         " << start << endl
	<< "::: end:           " << end << endl
	<< "::: overlap size:  " << signal.size()-(last-first) << endl;
}


CM_Unit::CM_Unit(BOSS::Config & config, MySQL_Handler * db, xercesc::DOMNode * n, const string & t)
  : ConmanUnit(config, db, n, t), start(0), end(0) {
  if (unsigned dur=unsigned((atof(BOSS::getAttrByName(node,"Dur").c_str())*16)+0.5)) {
    if (dur>0 && dur<MAXIMAL_PAUSE)
      signal = t_Signal(dur);
    else {
      signal = t_Signal(DEFAULT_PAUSE);
      xercesc::DOMElement* el = (xercesc::DOMElement *) node;
      BOSS::setAttribute(el, "Dur", DEFAULT_PAUSE_STRING);
    }
  }
  else {
    signal = t_Signal(DEFAULT_PAUSE);
    xercesc::DOMElement* el = (xercesc::DOMElement *) node;
    BOSS::setAttribute(el, "Dur", DEFAULT_PAUSE_STRING);
  }
}


CM_Unit & CM_Unit::operator=(const CM_Unit & other) {
        config = other.config;
        database = other.database;
        node = other.node;
        user_data = other.user_data;
        signal = other.signal;
        tkey = other.tkey;
        first = other.first;
        last = other.last;
        start = other.start;
        end = other.end;
        pause = other.pause;
        return *this;
}


/**
  *@author Karlheinz Stoeber
  */

void CM_Unit::loadFromFile(const string & file_name,
			   unsigned loadbegin,
			   unsigned loadend,
			   const unsigned & leftOL,
			   const unsigned & rightOL)
{
  static const char F_Name[] = "t_Signal * BOSS_ConMan::loadFromFile(const string &)";
  ifstream in(file_name.c_str());
  if(!in)
    throw BOSS::FileOpenError(F_Name, file_name);
  unsigned size;
  // Attempt to read the signal from entire stream
  t_Signal * tmp_signal(0);
  try {
    in.seekg(0, ios::end);
    size = in.tellg() / 2;
    in.seekg(0, ios::beg);
    tmp_signal = new t_Signal(in, size);
  }
  catch(const dsp::AllocationError & e) {
    cerr << e << endl;
  }
  catch(const dsp::StreamError & e) {
    cerr << e << endl;
  }
		
  try {  // If the overlap boundaries are out of file boundaries, automatically append zeroes to the signal
    unsigned append(0);
    if ((loadend+rightOL) > size) {
      append = (loadend+rightOL)-size;
      loadend = size;
    } else loadend += rightOL;
    if ((int(loadbegin)-int(leftOL)) < 0) {
      signal.cat(t_Signal(unsigned(0-(int(loadbegin)-int(leftOL)))));
      loadbegin = 0;
    } else loadbegin -= leftOL;
    signal.cat(t_Signal(*tmp_signal, loadbegin, loadend));
    if (append) signal.cat(t_Signal(append));
  }
  catch(const dsp::IndexError & e) {
    cerr << e << endl;
  }
  delete tmp_signal;
}

/** @} */ // end of boss_concat group
