/* dsp_short_time.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   A set of functions dividing a vector into a set of
   short time frames.
   @author Karlheinz Stoeber
*/

#ifndef __DSPSHORTTIMEH__
#define __DSPSHORTTIMEH__

#include "dsp_matrix.h"
#include "dsp_weight_window.h"
#include <math.h>

namespace dsp {

  typedef enum {
    RECTANGULAR,
    HANNING,
    HAMMING
  } TWindowType;

  // Parameters:
  // m           : the matrix containing the results. Each row contains a frame
  // v           : normally the signal on which the short time analysis is done
  // frame_size  : the window size of a short time frame
  // frame_shift : the window shift
  // w           : A weighting window (currently not implemented)
  // Beginning with i=0, each frame is centered around i*frame_shift. This leads to samples which
  // are not included in v. Each sample which isn't included in v is assumed to be zero.
  template < class TM, class TV > 
  void shortTime(matrix< TM > & m, const vector< TV > & v,
		 const unsigned & frame_size, const unsigned & frame_shift,
		 const TWindowType w = RECTANGULAR);

  // This function computes mainly the same as the function above. There are two additional
  // parameters:
  // left  : The left boundary of the short time analysis
  // right : The right boundary of the short time analysis
  // With aid of these boundaries, it is possible to do a short time analysis only for a part
  // of a signal.
  // If samples are outside the analysis interval, they will be taken from v. Only if the range
  // is outside of v will they be assumed to be zero
  template < class TM, class TV > 
  void shortTime(matrix< TM > & m, const vector< TV > & v,
		 const unsigned & frame_size, const unsigned & frame_shift,
		 const unsigned & left, const unsigned & right,
		 const TWindowType w = RECTANGULAR) throw(IndexError);

#include "dsp_short_time.cpp"

};

#endif
