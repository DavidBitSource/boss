/* boss_types.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include <boss_types.h>

// ######################################################
// # Input/output operators for t_SSPair
// ######################################################
ostream & operator << (ostream & out, const BOSS::t_SSPair & p)
{ return out << "(" << p.first << ", " << p.second << ")"; }
istream & operator >> (istream & in, BOSS::t_SSPair & p)
{ return in >> p.first >> p.second; }

// ######################################################
// # Input/output operators for t_USPair
// ######################################################
ostream & operator << (ostream & out, const BOSS::t_USPair & p)
{ return out << "(" << p.first << ", " << p.second <<")"; }
istream & operator >> (istream & in, BOSS::t_USPair & p)
{ return in >> p.first >> p.second; }

// ######################################################
// # Input/output operators for t_CSPair
// ######################################################
ostream & operator << (ostream & out, const BOSS::t_CSPair & p)
{ return out << "(" << p.first << ", " << p.second <<")"; }
istream & operator >> (istream & in, BOSS::t_CSPair & p)
{ return in >> p.first >> p.second; }

// ######################################################
// # Input/output and smaller operators for t_CCPair
// ######################################################
ostream & operator << (ostream & out, const BOSS::t_CCPair & p)
{ return out << "(" << p.first << ", " << p.second <<")"; }
istream & operator >> (istream & in, BOSS::t_CCPair & p)
{ return in >> p.first >> p.second; }
bool operator < (const BOSS::t_CCPair & a, const BOSS::t_CCPair & b)
{ if(a.first != b.first) return a.first < b.first; return a.second < b.second; }
