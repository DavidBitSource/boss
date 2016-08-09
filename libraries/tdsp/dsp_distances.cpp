/* dsp_distances.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   Various distance measures for vectors.
   @author Karlheinz Stoeber
*/


#ifndef SQR
 #define SQR(a) ((a)*(a))
#endif

#include "dsp_distances.h"

template< class T > T Euclid< T >::operator () (const vector < T > & a, const vector < T > & b) const
{
  register T t, sum(0);
  register unsigned i;
  for(i=0; i<a.size(); i++) {
    t = a.direct(i) - b.direct(i);
    sum += SQR(t);
  }
  return sqrt(sum);
}

template< class T > T Euclid< T >::operator () (const matrix < T > & a, const unsigned & ar, const matrix < T > & b, const unsigned & br) const
{
  register T t, sum(0);
  register unsigned i;
  for(i=0; i<a.col(); i++) {
    t = a.direct(ar, i) - b.direct(br, i);
    sum += SQR(t);
  }
  return sqrt(sum);
}


template< class T > T Correlation< T >::average(const vector < T > & a) const
{
  T sum(0);
  for(unsigned i=0; i<a.size(); i++)
    sum += a[i];
  return sum / T(a.size());
}

template< class T > T Correlation< T >::operator () (const vector < T > & a, const vector < T > & b) const
{
	static char F_Name[] = "template< class T > T Correlation< T >::operator () (const vector < T > & a, const vector < T > & b) const";
  if(a.size() != b.size()) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Size", a.size()));
    kvlist.push_back(TKVPair("Size", b.size()));
    throw IndexError(F_Name, kvlist);
  }

  T av_a, av_b, d_a, d_b, sp(0), sa(0), sb(0);
  av_a = average(a);
  av_b = average(b);

  for(unsigned i=0; i<a.size(); i++) {
    d_a = a[i] - av_a;
    d_b = b[i] - av_b;
    sp += d_a * d_b;
    sa += SQR(d_a);
    sb += SQR(d_b);
  }
  return sp / sqrt(sa * sb);
}

template< class T > T Correlation< T >::operator () (const matrix < T > & a, const unsigned & ar, const matrix < T > & b, const unsigned & br) const
{
  return (*this)(a[ar], b[br]);
}

#undef SQR
