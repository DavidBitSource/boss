/* moduletest.cpp    $Revision: 1.1 $ $Date: 2006/03/26 13:01:47 $
   Copyright (C) 2003 University of Bonn.
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
 * @file moduletest.cpp
 * @author Michal Holcik
 */

#include <iostream>
#include "moduletest.h"

using namespace std;

/**
 * @brief BOSS_OLA class constructor.
 */
ModuleTest::ModuleTest(BOSS::Config & __config, MySQL_Handler * __database)
	: BOSS::Module(__config, __database) {
  cout << "Construction of class ModuleTest ... ";
  cout << "OK" << endl;
}

/**
 * @brief Returns a description string of this module.
 */
string ModuleTest::getDescription() {
	return "BOSS_OLA: OLA Manipulation and concatentation module";
}

