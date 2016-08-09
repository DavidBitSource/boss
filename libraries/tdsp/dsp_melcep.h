/* dsp_melcep.h    $Revision: 1.3 $ $Date: 2005/12/17 16:06:11 $
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
   A class computing the MelCepstrum coefficients for a vector.
   @author Karlheinz Stoeber
*/

#ifndef __DSPMELCEPH__
#define __DSPMELCEPH__

#include "dsp_vector.h"
#include "dsp_fft.h"
#include "dsp_line.h"
#include "dsp_weight_window.h"
#include <math.h>

namespace dsp {

  template < class T > class MelCepstrum;

  // The class "TriagFilter" is used for the simulation of the filter bank of the MelCepstrum preprocessing.
  // Such a filter bank consists of triangle functions arranged equidistantly on the Mel scale.
  // TraigFilter represents a funciton equal to or greater than zero on the interval [l, r].
  // The function on the interval [l, r] represents a triangle, i.e. the function on the 
  // interval [l, m] is a line through the points (1,0) and (m,1). Accordingly, for the interval [m, r], (m,1) and (r,0) are valid.

  template< class T >
  class TriagFilter {
  private:
    // Values outside the boundaries are assumed to be zero
    T         l; // Left intervall boundary
    T         m; // Center
    T         r; // Right intervall boundary
    line< T > trl; // Filter coefficients [l, m] as line equation
    line< T > trr; // Filter coefficients [m, r] as line equation

  public:

    friend struct dsp::MelCepstrum< T >;

    TriagFilter();
    // Set the left and right boundaries and the center point of the filter.
    //   All boundaries are given in Hz.
    TriagFilter(const T & L, const T & M, const T & R);	
  
    // Filter coefficient at frequency "f_hz"
    T operator () (const T & f_hz) const; 
};

  // A class used for computation of MelCepstrum coefficients.
  // nrbands:	The number of filters used for weighting the fft coeffficents
  // l_hz:	Left frequency range
  // r_hz:	Right frequency range
  // ffts:	FFT size: The power of two used by the fft.
  // sr:	The sampling rate of the speech signal.
  // cc:	The number of MelCepstrum coefficents computed by this class
  template < class T >
  class MelCepstrum
  {

  private:

    vector< TriagFilter< T > > triags;
    fft< T >                   ft;
    T                          delta;
    unsigned                   cepcoeff;

    // Converts the frequency value x given in Hz to the Mel scale
    T  HzToMel(const T & x) const;
    // Converts the frequency value x given in Mel to the Hz scale
    T  MelToHz(const T & x) const;
    // Creates "triags.size()" filters in the frequency range [l_hz, r_hz]
    void createFilter(const T & l_hz, const T & r_hz);

  public:
  

    MelCepstrum(const unsigned nrbands, const T l_hz, const T r_hz, const unsigned ffts, const unsigned sr, const unsigned cc);

    /** Return the MelCepstrum coefficents for speech frame "d" */
    vector< T > operator () (const vector< T > & d, const bool energy = false) const;
  };

// why is this included here? --Hannes Rohde
#include "dsp_melcep.cpp"

};

#endif
