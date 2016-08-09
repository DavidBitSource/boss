/* boss_intonation_de.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   @author Philip Gross
*/

#ifndef BOSS_INTONATION_DE_H
#define BOSS_INTONATION_DE_H

//#include "dom/DOM.hpp"
#include "boss_utility/dom_tools.h"
#include "boss_intonation.h"
#include "boss_nn.h"
#include <iostream>
#include <string>
#include <fstream>
//#include <stdio.h>
//#include <stdlib.h>

/**
 * @addtogroup boss_intonation
 * @{
 */

class BOSS_Intonation_DE : public BOSS_Intonation {
private:
  void write_fuji(const xercesc::DOMNode * sentence) const;

public:
  struct t_Set
  {
    float blf[5];
    string syl;
    int Stress;
    int Word;
    float Pos;
    float Length;
    int PInt;
  };
  BOSS_NN decide,phrase,accent;
  int BLFs;
  vector <string> BLFStr;
  BOSS_Intonation_DE(BOSS::Config & _cl, MySQL_Handler * _db);
  ~BOSS_Intonation_DE();
  bool ParseBLF(const string & parse, float blf[5]) const;
  virtual void operator () (xercesc::DOMNode* sentence);
  string getDescription();
};

extern "C" BOSS::Module * create(BOSS::Config & _cl, MySQL_Handler * _db) {
	return new BOSS_Intonation_DE(_cl, _db);
}

extern "C" void destroy(BOSS_Intonation_DE * cm) {
	delete cm;
}

/** @} */ // end of boss_intonation group
#endif

