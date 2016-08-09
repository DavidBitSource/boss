/* ordinal.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   @author Joerg Broeggelwirth
*/

#ifndef ORDINAL_H
#define ORDINAL_H

#include "nominal.h"

/**
  *@author Joerg Broeggelwirth
  */

class ordinal : public BOSS_Numbers {	
private:
  nominal nom;

public:
  ordinal(const char* ofile, const char* nfile);
  //ordinal(const ordinal& o);
  virtual ~ordinal();
	
  virtual BOSS::t_SSPair roman(const string& input) const;
  BOSS::t_SSPairVector date(string date) const;
  virtual BOSS::t_SSPair operator () (string input_number) const;
};

#endif
