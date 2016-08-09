/* dsp_exceptions.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 1997-2001 University of Bonn.
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
   Classes for exception handling.
   @author Karlheinz Stoeber
*/

#include "dsp_exceptions.h"

using namespace dsp;

dsp::AllocationError::AllocationError(const string & F_Name, const unsigned & s) : who(F_Name), size(s)
{
#ifdef EXDEBUG
  cerr << *this << endl;
#endif
}

ostream & dsp::operator << (ostream & out, const AllocationError & tmp)
{ return out << "Memory allocation for size " << tmp.size << " failed in function: " << tmp.who; } 

StreamError::StreamError(const string & F_Name, const StreamErrorType & T) : who(F_Name), type(T)
{
#ifdef EXDEBUG
  cerr << *this << endl;
#endif
}

ostream & dsp::operator << (ostream & out, const StreamError & tmp)
{ 
  out << "Stream operation ";
  switch(tmp.type) {
  case StreamError::read:
    out << "read";
    break;
  case StreamError::seek:
    out << "seek";
    break;
  case StreamError::open:
    out << "open";
    break;
  case StreamError::write:
    out << "write";
    break;
  case StreamError::empty:
    out << "on empty vector";
    break;
  }
  out << " failed: " << " in " << tmp.who;
  return out;
}

IndexError::IndexError(const string & F_Name, const TKVPairList & kv) :  who(F_Name), kvlist(kv) 
{
#ifdef EXDEBUG
  cerr << *this << endl;
#endif
}

ostream & dsp::operator << (ostream & out, const IndexError & tmp)
{
  out << "Position, size or index error: ";
  TKVPairList::const_iterator it;
  for(it=tmp.kvlist.begin(); it!=tmp.kvlist.end(); it++)
    out << (*it).first << " = " << (*it).second << ", ";
  return out << " in " << tmp.who;
}
