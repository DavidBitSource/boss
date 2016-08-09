/* dsp_vector.h    $Revision: 1.3 $ $Date: 2005/12/17 16:06:11 $
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
   A class simulating mathematical vector operations.
   @author Karlheinz Stoeber
*/

#ifndef __DSPVECTORH__
#define __DSPVECTORH__

#include "dsp_exceptions.h"
#include <fstream>

namespace dsp {
  template < class T > class fft;
  template < class T > class matrix;
  template < class T > class Euclid;

  template < class T > class vector;
  template < class T > ostream& operator << (ostream& out, const vector< T > & v);

  template < class T >

  class vector {

  private:
    unsigned   Size;
    T        * Data;

    // Free allocated memory and set vector data to zero
    void emptyData();
    // Allocate memory and copy the content in V to it
    void alloccp(const T * V, const unsigned & s, const unsigned & l, const unsigned & r) throw(IndexError, AllocationError);
    // Allocate memory and copy the content in V.data to it
    void alloccp(const vector & V, const unsigned & l, const unsigned & r);
    // Allocate memory
    void alloc(const bool flag = true) throw(AllocationError) ;

  public:

    inline T & direct (const unsigned & i)
    { return Data[i]; }
    // Access to the components with range check
    inline T  direct (const unsigned & i) const
    { return Data[i]; }

    // ################################################################
    // ## Friends
    // ################################################################
 
    friend ostream & operator << <> (ostream & out, const vector< T > & v);
    friend class dsp::fft< T >;
    friend class dsp::matrix< T >;
    friend class dsp::Euclid< T >;

    // ################################################################
    // ## Constructors / Destructor
    // ################################################################
 
    // Create an empty vector
    vector();
    // Create a vector of dimension l
    vector(const unsigned & l);
    // Copy constructor
    vector(const vector & V);
    // Create a vector of dimension s. Copy the first s values of V
    vector(const vector & V, const unsigned s);
    // Create a vector of dimension s. Copy the first s values of V
    vector(const T * V, const unsigned s);
    // Create a vector of dimension r-l. Copy the values of V between l and r
    vector(const vector & V, const unsigned l, const unsigned r);
    // Create a vector of dimension r-l. Copy the values of V between l and r
    vector(const T * V, const unsigned l, const unsigned r);
    // Create a vector from stream in
    vector(istream & in) throw(StreamError);
    // Create a vector from stream in. Copy the first s values from the stream 
    vector(istream & in, const unsigned & s) throw(StreamError, IndexError);
    // Create a vector from stream in. Copy the values between l and r from the stream 
    vector(istream & in, const unsigned & l, const unsigned & r) throw(StreamError, IndexError);
    // Destructor
    ~vector();

    // ################################################################
    // ## Operators
    // ################################################################
    
    // Assignment operator
    vector & operator = (const vector & V);
    // Access to the components with range check and possibility to modify the component
    T & operator [] (const unsigned & i) throw(IndexError);
    // Access to the components with range check
    T  operator [] (const unsigned & i) const throw(IndexError);
    // Sum components
    vector< T > operator + (const vector< T > & v) const throw(IndexError);
    // Muliplication with a scalar
    vector< T > operator * (const T & v) const;
                              
    // ################################################################
    // ## Useful functions
    // ################################################################

    unsigned  size() const { return Size; }
    void      cat(const vector & v);
    void      clear();
    void      resize(const unsigned & s) throw (AllocationError);
    ostream & write(ostream & out);
    void      write(const string & file) throw (StreamError);
    const char * getData(){return (char *) Data;};
  };

#include "dsp_vector.cpp"

};

#endif
