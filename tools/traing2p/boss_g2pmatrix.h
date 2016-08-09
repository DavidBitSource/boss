/* boss_g2pmatrix.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   @author Stefan Breuer
*/

#ifndef BOSSG2PMATRIX_H
#define BOSSG2PMATRIX_H



/**
  *@author Stefan Breuer
  */

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "boss_utility/boss_utility.h"
#include "tdsp/dsp_matrix.h"

typedef map< string, unsigned > t_GPTableMap;
typedef pair< unsigned, double > t_UDPair;

class BOSS_g2pMatrix {
public:
  BOSS_g2pMatrix(const BOSS::t_SVector & gv, const BOSS::t_SVector & pv);
  ~BOSS_g2pMatrix();
  void add_value(const string & graph, const string & phon, const unsigned & value);
  void set_value(const string & graph, const string & phon, const double & value);
  void compute_prob();
  t_UDPair get_value(const string & graph, const string & phon);
  BOSS::t_SVector get_graphemes();
  BOSS::t_SVector get_phones();
  BOSS::t_SVector get_matrix();
protected:
  t_GPTableMap gm;
  t_GPTableMap pm;
  dsp::matrix<t_UDPair> matrix;
  void t_SVectorToMap(const BOSS::t_SVector & v, t_GPTableMap & m);
};


#endif
