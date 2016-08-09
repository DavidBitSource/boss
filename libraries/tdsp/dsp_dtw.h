/* dsp_dtw.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

// Setting emacs mode -*-c++-*-

#ifndef __DSPDTWH__
#define __DSPDTWH__

#include "dsp_matrix.h"

#include <vector>
typedef std::pair< unsigned, unsigned > t_UUPair;
typedef std::vector< t_UUPair >         t_UUPairVector;

namespace dsp {

  template < class T, class COST > T dtw(const matrix< T > & a, const matrix < T > & b, const COST & cost, const T WEIGHT = 2.0);
  template < class T, class COST > T dtw(const matrix< T > & a, const matrix < T > & b, const COST & cost, t_UUPairVector & path, const T WEIGHT = 2.0);

#include "dsp_dtw.cpp"

};

#endif
