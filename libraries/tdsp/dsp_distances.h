/* dsp_distances.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2001 University of Bonn.
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

#ifndef __DSPDISTANCESH__
#define __DSPDISTANCESH__

#include "dsp_vector.h"

namespace dsp {

  template < class T >
  struct Euclid {
    T operator () (const vector < T > & a, const vector < T > & b) const;
    T operator () (const matrix < T > & a, const unsigned & ar, const matrix < T > & b, const unsigned & br) const;
  };

  template < class T >
  struct Correlation {
    T average(const vector < T > & a) const;
    T operator () (const vector < T > & a, const vector < T > & b) const;
		T operator () (const matrix < T > & a, const unsigned & ar, const matrix < T > & b, const unsigned & br) const;
  };
#include "dsp_distances.cpp"

};


#endif
