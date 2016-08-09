/* dsp_weight_window.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   A set of functions to weight the samples of short time windows.
   @author Karlheinz Stoeber
*/

#ifndef __DSPWEIGHTWINDOWH__
#define __DSPWEIGHTWINDOWH__

#include <math.h>

namespace dsp {

  // Windows containing N samples centered around zero

  double hanning(const int & n, const int & N);

  double hamming(const int & n, const int & N, const double alpha = 0.54);
 
};

#endif
