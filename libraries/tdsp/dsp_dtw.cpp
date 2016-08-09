/* dsp_dtw.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 1998 University of Bonn.
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
   dtw: (dynamic time warping) Compute a shortest path in a distance matrix.
   @author Karlheinz Stoeber
*/

#include "dsp_dtw.h"

template < class T > T min3(const T & a, const T & b, const T & c)
{ 
  T t = a < b ? a : b;
  return t < c ? t : c;
}

template < class T>
T pmin3(const T a, const T b, const T c, char & f)
{
  T h;
  if(a < b) { h = a; f = 0; }
  else      { h = b; f = 1; }
  if(c < h) { h = c; f = 2; }
  return h;
}

// Decode the path from pm. pm includes the values {0, 1, 2}, where 0 denotes
// a horizontal, 1 a diagonal and 2 a vertical step
inline void trackPath(const unsigned i, const unsigned j, const matrix < char > & pm, t_UUPairVector & p)
{
  if(i==0 && j==0) {
    p.push_back(t_UUPair(i, j));
    return;
  }

  switch(pm(i, j)) {
  case 0:
    trackPath(i-1, j, pm, p);
    break;
  case 1:
    trackPath(i-1, j-1, pm, p);
    break;
  case 2:
    trackPath(i, j-1, pm, p);
    break;
  };
  p.push_back(t_UUPair(i, j));
}

template < class T, class COST >
T dtw(const matrix< T > & a, const matrix < T > & b, const COST & cost, const T WEIGHT = 2.0)
{ 
  vector< T > *h[2], *t;
  h[0] = new vector< T >(a.row());
  h[1] = new vector< T >(a.row());
  register T c;
  
  (*h[0]).direct(0) = cost(a, 0, b, 0) * WEIGHT;
  
  for(unsigned i=1; i<a.row(); i++)
    (*h[0]).direct(i) = (*h[0]).direct(i-1) + cost(a[i], b[0]);
  
  t = h[0]; h[0] = h[1]; h[1] = t;
  
  for(unsigned j=1; j<b.row(); j++) {
    (*h[0]).direct(0) = (*h[1]).direct(0) + cost(a, 0, b, j);
    for(unsigned i=1; i<a.row(); i++) {
      c = cost(a, i, b, j);
      (*h[0]).direct(i) = min3((*h[0]).direct(i-1)+c, (*h[1]).direct(i-1)+WEIGHT*c, (*h[1]).direct(i)+c);
    }
    t = h[0]; h[0] = h[1]; h[1] = t;
  }
  c = (*h[1]).direct(a.row()-1);
  
  delete h[0];
  delete h[1];
  
  return c / (a.row() + b.row());
}

template < class T, class COST >
T dtw(const matrix< T > & a, const matrix < T > & b, const COST & cost, t_UUPairVector & path, const T WEIGHT = 2.0)
{ 
  vector< T > *h[2], *t;
  dsp::matrix< char >  pm(a.row(), b.row());
  h[0] = new vector< T >(a.row());
  h[1] = new vector< T >(a.row());
  register T c;
  register char f;
  
  (*h[0]).direct(0) = cost(a, 0, b, 0) * WEIGHT;
  pm(0, 0) = -1;

  for(unsigned i=1; i<a.row(); i++) {
    (*h[0]).direct(i) = (*h[0]).direct(i-1) + cost(a[i], b[0]);
    pm(i, 0) = 0;
  }
  
  t = h[0]; h[0] = h[1]; h[1] = t;
  
  for(unsigned j=1; j<b.row(); j++) {
    (*h[0]).direct(0) = (*h[1]).direct(0) + cost(a, 0, b, j);
    pm(0, j) = 2;
    for(unsigned i=1; i<a.row(); i++) {
      c = cost(a, i, b, j);
      (*h[0]).direct(i) = pmin3((*h[0]).direct(i-1)+c, (*h[1]).direct(i-1)+WEIGHT*c, (*h[1]).direct(i)+c, f);
      pm(i, j) = f;
    }
    t = h[0]; h[0] = h[1]; h[1] = t;
  }
  c = (*h[1]).direct(a.row()-1);
  
  delete h[0];
  delete h[1];

  trackPath(a.row()-1, b.row()-1, pm, path);
  
  return c / (a.row() + b.row());
}
