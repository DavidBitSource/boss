/* dsp_convolution.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   A set of functions used for signal convolution.
   @author Karlheinz Stoeber
*/

#ifndef __DSPCONVULTIONH__
#define __DSPCONVULTIONH__

#include <fstream>

#include "dsp_vector.h"

namespace dsp {

  // Routine for reading the filter coefficients from file "name"
  template< class T > vector< T > ReadFilter(const string & name);

  // Standard calculation of the folding operation
  template< class T > vector<T> Conv(const vector<T> & signal, const vector<T> & filter, const unsigned fzero);

  // ConvSym folds the signal with a symmetric filter
  // Only half of the filter coefficients are given from 0 to n.
  template< class T > vector<T> ConvSym(const vector<T> & signal, const vector<T> & filter);

  // The following function folds a signal with a halfband filter.
  // The sample rate is reduced or increased by the factor two, respectively.
  // A halfband filter has the following properties:
  // - Symmetric to zero
  // - All even filter coefficients except n=0 are equal to zero
  // The functions exploit these properties for the redutction of the number of folding operations.
  // The folding type is linear, i.e. all values outside the signal interval are set to zero.
  // The vector "filter" contains the filter coefficients from n=0 to n=N
  template< class T > vector<T> ConvHbDown(const vector<T> & signal, const vector<T> & filter);
  template< class T > vector<T> ConvHbUp(const vector<T> & signal, const vector<T> & filter);


#include "dsp_convolution.cpp"

};

#endif
