/* dsp_fft.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   A class for computing the fft for a vector. The computation is table based.
   @author Wolfgang Hess
   @author Karlheinz Stoeber
*/

#ifndef __DSPFFTH__
#define __DSPFFTH__

#include "dsp_vector.h"
#include <math.h>

namespace dsp {

  template < class T >
  class fft
  {
  private:
    
    double * sinus;
    double * cosinus;      
    int    * mirror;
    unsigned Size;
    
    void     mbtrbtr (T * b, T * b1, int * ispieg, int * ispg1) const;
    void     mshfbgr (const int n, T * b) const;
    void     mff2ti  (const int iii, int nn, T * b0, T * b1, T * b2) const;
    void     mff2tr  (const int iii, int nn, T * b0, T * b1, T * b2) const;
    int      mffir2  (int n, T * b) const;
    int      mfftr2  (const int n, T * b) const;
    int      mtabbr2 (int ldn, int mode) const;
    unsigned mbitrev (unsigned ldn, unsigned inval) const;
    
  public:

    typedef enum { FORWARD, REVERSE } FFTType;

    fft(const unsigned & size);
    ~fft();
    vector< T > operator () (const vector< T > & data, const FFTType flag = FORWARD) const;
    unsigned size() const { return Size; }
  };

#include "dsp_fft.cpp"

};

#endif
















