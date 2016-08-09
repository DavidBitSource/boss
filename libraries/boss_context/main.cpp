/* main.cpp (boss_context)    $Revision: 1.3 $ $Date: 2005/12/10 20:33:54 $
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
   @author Stefan Breuer
*/

#include "boss_context.h"
#include "boss_utility/boss_config.h"

int main(int argc, char *argv[]) {
  BOSS::Config cl(argc, argv, "usage");
  string s;
  string ccfile = cl("-ccfile").optionarg;
  BOSS_context context(ccfile);
  cin >> s;
  cout << s << "\011" <<  context.left1(s) << "\011" << context.right1(s)
       << "\011" <<  context.left2(s) << "\011" << context.right2(s) << endl;
}
