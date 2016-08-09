/* boss_context.cpp    $Revision: 1.4 $ $Date: 2005/12/10 20:33:54 $
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/**
   @author Stefan Breuer
*/

#include "boss_context.h"

BOSS_context::BOSS_context(string & file) {
  ifstream in(file.c_str());
  int i=0;
  string phon,cleft1,cright1,cleft2,cright2;
  while (in) {
    i++;
    phon = cleft1 = cright1 = cleft2 = cright2 = "";
    in >> phon >> cleft1 >> cright1 >> cleft2 >> cright2;
    if (phon != "" && (cleft1=="" || cright1=="" || cleft2=="" || cright2==""))
      cerr << "BOSS_context: Error in data file " << file << ", line " << i << endl;
    if (phon != "" && cleft1 != "" && cright1 !="" && cleft2 != "" && cright2 !="") {
      struct CCStruct cc = {cleft1,cright1,cleft2,cright2};
      cmap[phon]=cc;
    }
  }
  in.close();
}
