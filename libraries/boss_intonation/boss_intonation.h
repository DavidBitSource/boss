/* boss_intonation.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2002 University of Bonn.
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
 * @file boss_intonation.h
 * @author Philip Gross
 */

#ifndef BOSS_INTONATION_H
#define BOSS_INTONATION_H

#include "boss_utility/boss_module.h"

/**
 * @defgroup boss_intonation BOSS Intonation Module
 * @{
 */

/**
 * @brief Base class (language indepenedent) of the intonation generation modules.
 */
class BOSS_Intonation : public BOSS::Module {
private:

public:
  BOSS_Intonation(BOSS::Config & __cl, MySQL_Handler * __data_base): Module(__cl, __data_base) {}
  virtual ~BOSS_Intonation() {}
  virtual void operator () (xercesc::DOMNode * sentence) = 0;
};

/** @} */ // end of boss_intonation group
#endif
