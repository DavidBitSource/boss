/* boss_types.cpp    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
   Copyright (C) 1999 University of Bonn.
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


#ifndef __BOSS_TYPESH__
#define __BOSS_TYPESH__


// Standard includes
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace BOSS
{

// Short cuts for basic types
typedef unsigned char       t_UChar;
typedef unsigned short      t_UShort;
typedef unsigned int        t_UInt;

// Enum section

// Pair section
typedef pair< string, string >     t_SSPair;
typedef pair< unsigned, string >   t_USPair;
typedef pair< unsigned, unsigned > t_UUPair;
typedef pair< t_UChar, string >    t_CSPair;
typedef pair< t_UChar, t_UChar >   t_CCPair;

// Vector section
typedef vector< string >            t_SVector;
typedef vector< unsigned >          t_UVector;
typedef vector< t_USPair >          t_USPairVector;
typedef vector< t_SSPair >          t_SSPairVector;
typedef vector< t_UUPair >          t_UUPairVector;

// Iterator section
typedef t_UVector::iterator         t_UVectorIt;
typedef t_UVector::const_iterator   t_UVectorCIt;
// Map section

// Mixed section

/* A struct for storing file information (path, name, extension) */

  struct Fileinfo {
    string path;
    string base;
    string ext;
    bool exists;
  };
}

// Comparing operators
bool operator < (const BOSS::t_CCPair & a, const BOSS::t_CCPair & b);

// Stream operator prototypes
ostream & operator << (ostream & out, const BOSS::t_SSPair & p);
istream & operator >> (istream & in,  BOSS::t_SSPair & p);

ostream & operator << (ostream & out, const BOSS::t_USPair & p);
istream & operator >> (istream & in,  BOSS::t_USPair & p);

ostream & operator << (ostream & out, const BOSS::t_CSPair & p);
istream & operator >> (istream & in,  BOSS::t_CSPair & p);

ostream & operator << (ostream & out, const BOSS::t_CCPair & p);
istream & operator >> (istream & in,  BOSS::t_CCPair & p);

#endif
