/* boss_g2pmatrix.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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

#include "boss_g2pmatrix.h"

BOSS_g2pMatrix::BOSS_g2pMatrix(const BOSS::t_SVector & gv, const BOSS::t_SVector & pv) {
  t_SVectorToMap(gv, gm);
  t_SVectorToMap(pv, pm);
  matrix = dsp::matrix<t_UDPair>(gm.size(),pm.size());
}
BOSS_g2pMatrix::~BOSS_g2pMatrix() {
}
void BOSS_g2pMatrix::t_SVectorToMap(const BOSS::t_SVector & v, t_GPTableMap & m) {
//  cerr << "Called BOSS_g2pMatrix::t_SVectorToMap" << endl;
  for (unsigned i=0;i<v.size();i++) {
    m[v[i]]=0;
  }
  //Index numbers are assigned in a second loop, to make sure that indexes are according
  // to sort order (even if the input vector was unsorted)!
  unsigned j=0;
  for (t_GPTableMap::iterator I = m.begin();I != m.end();I++)
    	(*I).second=j++;
}
void BOSS_g2pMatrix::add_value(const string & graph, const string & phon, const unsigned & value) {
// cerr << "Called BOSS_g2pMatrix::add_value" << endl;
  (matrix(gm[graph],pm[phon])).first += value;
}
void BOSS_g2pMatrix::set_value(const string & graph, const string & phon, const double & value) {
// cerr << "Called BOSS_g2pMatrix::add_value" << endl;
  (matrix(gm[graph],pm[phon])).second = value;
}
t_UDPair BOSS_g2pMatrix::get_value(const string & graph, const string & phon) {
// cerr << "Called BOSS_g2pMatrix::get_value" << endl;
    return matrix(gm[graph],pm[phon]);
}
void BOSS_g2pMatrix::compute_prob() {
  // cerr << "Called BOSS_g2pMatrix::compute_prob()" << endl;
    unsigned sum;
    for (unsigned i=0;i<gm.size();i++) {
        sum = 0;
        for (unsigned j=0;j<pm.size();j++) {
            sum += (matrix(i,j)).first;
        }
        for (unsigned j=0; j<pm.size(); j++) {
            (matrix(i,j)).second = (double((matrix(i,j)).first)/double(sum))*100.00;
        }
    }
}
BOSS::t_SVector BOSS_g2pMatrix::get_graphemes(){
    // cerr << "Called BOSS_g2pMatrix::get_graphemes" << endl;
    BOSS::t_SVector res;
    for (t_GPTableMap::iterator Igm = gm.begin();Igm != gm.end();Igm++)
    	res.push_back((*Igm).first);
    return res;
}
BOSS::t_SVector BOSS_g2pMatrix::get_phones(){
// cerr << "Called BOSS_g2pMatrix::get_phones" << endl;
  BOSS::t_SVector res;
  for (t_GPTableMap::iterator Ipm = pm.begin();Ipm != pm.end();Ipm++)
    	res.push_back((*Ipm).first);
  return res;
}
BOSS::t_SVector BOSS_g2pMatrix::get_matrix() {
    // cerr << "Called BOSS_g2pMatrix::get_matrix" << endl;
    BOSS::t_SVector g = get_graphemes();
    BOSS::t_SVector p = get_phones();
    BOSS::t_SVector res;
    for (unsigned i=0;i<g.size();i++) {
        for (unsigned j=0;j<p.size();j++) {
            ostringstream s;
            s << g[i] << "  " << p[j] << "   " << (get_value(g[i],p[j])).first << " " << (get_value(g[i],p[j])).second;
            res.push_back(s.str());
        }
    }
    return res;
}
