/* moduletest.h    $Revision: 1.1 $ $Date: 2006/03/26 13:01:47 $
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
 * @file moduletest.h
 * @author Michal Holcik
 */

#ifndef MODULE_TEST_H
#define MODULE_TEST_H

#include <boss_mysql/boss_mysql_handler.h>
//#include <boss_utility/dom_userdata.h>
//#include <boss_utility/dom_tools.h>
//#include <boss_utility/boss_exceptions.h>
//#include <boss_utility/boss_utility.h>
#include <boss_utility/boss_module.h>
#include <boss_utility/boss_config.h>
#include <string>
//#include <xercesc/dom/DOM.hpp>

/** @defgroup module_test Module Test
 * @{
 */

/**
 *  @brief A module for concatenation and manipulation of speech units.
 */
class ModuleTest : public BOSS::Module {

 public:

  ModuleTest(BOSS::Config & __config, MySQL_Handler * __database);
  virtual std::string getDescription();
  virtual void operator () (xercesc::DOMNode * sentence) { }
  
};

extern "C" ModuleTest * create(BOSS::Config config, MySQL_Handler * database) {
	return new ModuleTest(config, database);
}

extern "C" void destroy(ModuleTest * cm) {
	delete cm;
}

/** @} */ //end of module_test group

#endif

