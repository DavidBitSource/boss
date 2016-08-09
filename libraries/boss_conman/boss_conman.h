/* boss_conman.h    $Revision: 1.3 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_conman.h
 * @author Stefan Breuer
 */

#ifndef BOSS_SYNTHESIS_MODULE_HPP
#define BOSS_SYNTHESIS_MODULE_HPP

//#include <xercesc/dom/DOM.hpp>
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_module.h"

typedef dsp::vector< short > t_Signal;

/// this is an interface class for all conman synthesis modules

class BOSS_ConMan : public BOSS::Module {
 public:
  BOSS_ConMan(BOSS::Config & __config, MySQL_Handler * __database):
	BOSS::Module(__config, __database) {}
  virtual ~BOSS_ConMan() {}
  
  void writeSignalToXML(t_Signal & signal, xercesc::DOMNode* sentence) {
	DOM_UserData * user_data;
	if((user_data = (DOM_UserData*)BOSS::getUserData(sentence)) == 0) {
		user_data = new DOM_UserData();
		BOSS::setUserData(sentence, user_data);
	}
	user_data->setSignal(signal);
  }
  
};

#endif

