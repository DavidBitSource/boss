/* unit_cost.h    $Revision: 1.3 $ $Date: 2007/04/16 09:43:33 $
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
 * @file cost.h
   @author Karlheinz Stoeber
   @author Stefan Breuer
   @author Joerg Broeggelwirth
*/

#ifndef COST_H
#define COST_H

#include "boss_node.h"
#include "boss_utility/boss_config.h"

using namespace std;

/** @addtogroup boss_unitselection
 * @{
 */

/**
 * @brief Abstract interface to language dependent CostXX classes.
 * @author Michal Holcik
 */
class Cost {
 protected:
  /// System wide configuration container.
  BOSS::Config & cl;

 public:
  /// @brief Cost class constructor.
  Cost(BOSS::Config & __cl) : cl(__cl) {}
  /// @brief Cost destructor.
  virtual ~Cost() {}
  /// @brief Compute transition cost of two BOSS_Nodes.
  virtual float operator () (const BOSS_Node * l, const BOSS_Node * r) = 0;
  /// @brief Compute cost of one BOSS_Node.
  virtual double operator () (const BOSS_Node * u) const = 0;

};

/** @} */ //end of boss_unitselection group
#endif

