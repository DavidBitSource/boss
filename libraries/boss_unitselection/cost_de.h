/* unit_cost.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
   Copyright (C) 2000 University of Bonn.
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
 * @file cost_de.h
   @author Karlheinz Stoeber
   @author Stefan Breuer
   @author Joerg Broeggelwirth
*/

#ifndef COST_DE_H
#define COST_DE_H

#include "tdsp/dsp_distances.h"
#include "boss_utility/boss_config.h"
#include "cost.h"
#include "boss_node.h"

using namespace std;

/** @addtogroup boss_unitselection
 * @{
 */

/**
 * @brief Cost class used for german language.
 */
class CostDE: public Cost {
 private:
  dsp::Euclid< float > euclid;
  inline unsigned translateStress (unsigned s) const {
    if (s == 1) s = 2;
    else if (s == 2) s = 1;
    return s;
  }
  double stress_cost(const BOSS_Node * u) const;
  double phrasing_cost(const BOSS_Node * u) const;
  double duration_cost(const BOSS_Node * u) const;
  double treal_cost(const BOSS_Node * u) const;
  float mel_dist(const BOSS_Node * l, const BOSS_Node * r) const;
  //  float lpc_dist(const BOSS_Node & l, const BOSS_Node & r) const;
  float f0_dist(const BOSS_Node * l, const BOSS_Node * r) const;
  bool isSibilant(const string & s) const;
  bool isNasal(const string & s) const;
 public:
   CostDE(BOSS::Config & __cl);
  ~CostDE();
  float operator () (const BOSS_Node * l, const BOSS_Node * r);
  double operator () (const BOSS_Node * u) const;
};

/** @} */ //end of boss_unitselection group
#endif

