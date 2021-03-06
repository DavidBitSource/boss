/* dsp_matrix.cpp    $Revision: 1.3 $ $Date: 2005/12/17 16:06:11 $
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
   A class simulating mathematical matrix operations.
   @author Karlheinz Stoeber
*/

#ifndef __DSPMATRIXH__
#define __DSPMATRIXH__

#include "dsp_vector.h"

namespace dsp {

  template < class T > class Euclid;
  
  template < class T > class matrix;

  template < class T > ostream& operator << (ostream & out, const matrix< T > & v);
  
  template < class T >
  class matrix {

  private:
    vector< T > Data;
    unsigned    Row;
    unsigned    Col;

    // Access to the components without range check and possibility to modify the component
    inline T & direct(const unsigned & r, const unsigned & c)
    { return Data.direct(r*Col+c); }
    // Access to the components without range check
    inline T   direct(const unsigned & r, const unsigned & c) const
    { return Data.direct(r*Col+c); }

  public:

    // ################################################################
    // ## Friends
    // ################################################################
 
    friend ostream & operator << <> (ostream & out, const matrix< T > & v);
    friend class dsp::Euclid< T >;

    // ################################################################
    // ## Constructors / Destructor
    // ################################################################
 
    // Create an empty matrix
    matrix();
    // Create a matrix of dimension r, c
    matrix(const unsigned & r, const unsigned & c);
    // Create a matrix of dimension r, c from stream in
    matrix(istream & in, const unsigned & left, const unsigned right, const unsigned & c) throw(IndexError);
    // Returns a row as vector
    vector< T > operator [] (const unsigned & r) const throw(IndexError);
    // Access to the components with range check and possibility to modify the component
    T & operator () (const unsigned & r, const unsigned & c) throw(IndexError);
    // Access to the components with range check
    T   operator () (const unsigned & r, const unsigned & c) const throw(IndexError);

    // ################################################################
    // ## Useful functions
    // ################################################################

    unsigned row() const { return Row; }
    unsigned col() const { return Col; }
    void clear();
    void data() const { cerr << Data << endl; }
 };

#include "dsp_matrix.cpp"

};

#endif
