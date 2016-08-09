/* boss_tools.h    $Revision: 1.4 $
   Copyright (C) 2004 University of Bonn.
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

#ifndef BOSS_MODULE_H
#define BOSS_MODULE_H


#include "xercesc/dom/DOM.hpp"
#include "boss_utility/boss_config.h"
#include "boss_mysql/boss_mysql_handler.h"
#include <string>

using namespace std;

namespace BOSS{
  
  class Module {
  protected:
   BOSS::Config & cl;
   MySQL_Handler * data_base;
    
  public:
    Module(BOSS::Config & __cl, MySQL_Handler * __data_base): cl(__cl), data_base(__data_base) {}
    virtual void operator()(xercesc::DOMNode*) = 0;
    virtual string getDescription() = 0;
  };
}

// synthesis module class factories types
typedef BOSS::Module * create_module(BOSS::Config &, MySQL_Handler *);
typedef void destroy_module(BOSS::Module *);

#endif
