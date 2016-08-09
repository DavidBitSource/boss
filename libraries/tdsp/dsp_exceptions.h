/* dsp_exceptions.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#ifndef __DSPEXCEPTIONS__
#define __DSPEXCEPTIONS__

using namespace std;

#include <iostream>
#include <string>
#include <list>

typedef pair< string, unsigned > TKVPair; // A key/value pair
typedef list< TKVPair > TKVPairList;

namespace dsp {
  class AllocationError {
  private:
    string   who;
    unsigned size;
  public:
    AllocationError(const string & F_Name, const unsigned & s);
    friend ostream & operator << (ostream & out, const AllocationError & tmp);
  };

  class StreamError {
  public:
    typedef enum {
      seek,
      read,
      open,
      write,
      empty
    } StreamErrorType;
  private:
    string          who;
    StreamErrorType type;
  public:
    StreamError(const string & F_Name, const StreamErrorType & T);
    friend ostream & operator << (ostream & out, const StreamError & tmp);
  };

  class IndexError {
  public:
  private:
    string         who;
    TKVPairList    kvlist;
  public:
    IndexError(const string & F_Name, const TKVPairList & kv);
    friend ostream & operator << (ostream & out, const IndexError & tmp);
  };

};

#endif
