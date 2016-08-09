/* dsp_line.cpp    $Revision: 1.3 $ $Date: 2005/12/17 16:06:11 $
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
   A class representing a mathematical line equation.
   @author Karlheinz Stoeber
*/

#include "dsp_line.h"

// Least square approximation
template < class T > line<T>::line(const vector< line<T>::point > & l)
{
  line<T>::point t;
  double xi = 0, yi = 0, xi2 = 0, xiyi = 0;
  for(unsigned i=0; i<l.size(); i++) {
    t     = l[i];
    xi   += t.x; 
    yi   += t.y;
    xi2  += t.x * t.x;
    xiyi += t.x * t.y;
  }
  double D    = l.size() * xi2 - xi * xi;
  
  if (D==0) {
    cerr << "dsp::leastSquare: Warning matrix singular: " << l.size() << endl;
    m = b = 0;
  }
  else {
    b = (yi * xi2        - xi * xiyi) / D;
    m = (l.size() * xiyi - xi * yi)   / D;
  }
}

