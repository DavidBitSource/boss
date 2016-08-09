/* dsp_line.h    $Revision: 1.3 $ $Date: 2005/12/17 16:06:11 $
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

#ifndef __DSPLINEH__
#define __DSPLINEH__

/*
 *  <line: A class representing a mathematical line equation>
 *  Copyright (C) 1997-2001 <Karlheinz Stoeber>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  email: kst@ikp.uni-bonn.de
 */

#include "dsp_vector.h"

namespace dsp {

  template < class T >
  class line {
  public:
    struct point {
      T x;
      T y;
      point() : x(0), y(0) {}
      point(const T & X, const T & Y) : x(X), y(Y) {}
    };

  private:
    T m; // ascent
    T b; // offset
	
  public:
    // Empty line
    line() : m(0), b(0) {}
    // Construct a line equation from two points
    line(const line<T>::point & p1, const line<T>::point & p2)
      { m = (p1.y - p2.y) / (p1.x - p2.x); b = p2.y - m * p2.x; }
    // Least square approximation
    line(const vector< line<T>::point > & l);
    // Evaluate line
    T operator () (const T & x) const { return m * x + b; }
    // Get the ascent
    T ascent() const { return m; }
    // Get the offset
    T offset() const { return b; }
  };
};		

#endif

