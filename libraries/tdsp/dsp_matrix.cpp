/* dsp_matrix.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   A class simulating mathematical matrix operations.
   @author Karlheinz Stoeber
*/

#include "dsp_matrix.h"

using namespace dsp;

template< class T > ostream & operator << (ostream & out, const matrix< T > & m)
{
  unsigned i;
  for(i=0; i<m.Row-1; i++)
    out << vector< T >(m.Data, i*m.Col, (i+1)*m.Col) << endl;
  return out << vector< T >(m.Data, i*m.Col, (i+1)*m.Col);
}

template < class T > matrix< T >::matrix() : Row(0), Col(0) {}

template < class T > matrix< T >::matrix(const unsigned & r, const unsigned & c) : Data(r*c), Row(r), Col(c) {}

template < class T > matrix< T >::matrix(istream & in, const unsigned & left, const unsigned right, const unsigned & c) throw(IndexError) : Row(right-left), Col(c)
{
  static const string F_Name("matrix< T >::matrix(istream & in, const unsigned & left, const unsigned right, const unsigned & c)");
  if(left >= right) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Left",  left));
    kvlist.push_back(TKVPair("Right", right));
    throw IndexError(F_Name, kvlist);
  }
  Data = vector< T >(in, left*Col, right*Col);
}

template< class T > vector< T > matrix<T>::operator [] (const unsigned & r) const throw(IndexError)
{
  static const string F_Name("vector< T > matrix< T >::operator [] (const unsigned &) const");
  if(r>=Row) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Row", Row));
    kvlist.push_back(TKVPair("row", r));
    throw IndexError(F_Name, kvlist);
  }
  return vector< T >(Data, r*Col, (r+1)*Col);
}                                         

template< class T > T & matrix<T>::operator () (const unsigned & r, const unsigned & c) throw(IndexError)
{
  static const string F_Name("T & matrix< T >::operator () (const unsigned &, const unsigned &)");
  if(r>=Row || c>=Col) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Row", Row));
    kvlist.push_back(TKVPair("row", r));
    kvlist.push_back(TKVPair("Col", Col));
    kvlist.push_back(TKVPair("col", c));
    throw IndexError(F_Name, kvlist);
  }
  return Data.direct(r*Col+c);
}

template< class T > T matrix<T>::operator () (const unsigned & r, const unsigned & c) const throw(IndexError)
{
  static const string F_Name("T matrix< T >::operator () (const unsigned &, const unsigned &) const");
  if(r>=Row || c>=Col) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Row", Row));
    kvlist.push_back(TKVPair("row", r));
    kvlist.push_back(TKVPair("Col", Col));
    kvlist.push_back(TKVPair("col", c));
    throw IndexError(F_Name, kvlist);
  }
  return Data.direct(r*Col+c);
}


template< class T > void matrix<T>::clear()
{
  Data.clear();
  Row = Col = 0;
}
