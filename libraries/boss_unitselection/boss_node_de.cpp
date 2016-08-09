/* boss_node.cpp    $Revision: 1.3 $ $Date: 2006/12/15 00:19:00 $
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
 * @file boss_node_de.cpp
   @author Karlheinz Stoeber
   @author Stefan Breuer
*/

#include "boss_node_de.h"

/** @addtogroup boss_unitselection
 * @{
 */

BOSS_NodeDE::BOSS_NodeDE(BOSS::Config & __cl)
	: BOSS_Node(__cl),
	Num(0), tkey(""), treal(""),
	cleft(""), cright(""), pmode(""), pint(0),
	first(0), last(0), stress(0),
	lf0(0), rf0(0), avgf0(0), rms(0) {}

BOSS_NodeDE::BOSS_NodeDE(BOSS::Config & __cl,const Node_Type & t)
	: BOSS_Node(__cl, t),
	Num(0), tkey(""), treal(""),
	cleft(""), cright(""), pmode(""), pint(0), first(0), last(0), stress(0), lf0(0), rf0(0), avgf0(0), rms(0)
{
	if(t==BOSS_Node::INITIAL) tkey="INITIAL/FINAL";
}


BOSS_NodeDE::BOSS_NodeDE(BOSS::Config & __cl,	const xercesc::DOMNode * node, const Node_Type & t, const unsigned & p)
	: BOSS_Node(__cl, node, t, p),
	lmel(11), rmel(11)
{
  if (BOSS::getAttrByName(node, "TKey") == "$p") {
	Num = 0;
	tkey = "$p";
	treal = "$p";
	lf0 = 0;
	rf0 = 0;
	avgf0 = 0;
	rms = 0;
  } else {
    Num    = atoi(getResult("Num").c_str());
    tkey   = getResult("TKey");
    treal  = getResult("TReal");	
    cleft  = getResult("CLeft");
    cright = getResult("CRight");
    first = atoi(getResult("First").c_str());
    last = atoi(getResult("Last").c_str());
    if (t != BOSS_Node::WORD) {
      stress = atoi(getResult("Stress").c_str());
    }
    pmode = getResult("PMode");
    pint  = atoi(getResult("PInt").c_str());
    if (t == BOSS_Node::PHONE) {
      lf0 = atof(getResult("LF0").c_str());
      rf0 = atof(getResult("RF0").c_str());
      avgf0 = atof(getResult("AVGF0").c_str());
      rms = atof(getResult("RMS").c_str());
    }
    char buf[10];
    if (cl("-spec").optionarg=="melc") {
      for(unsigned i=0; i<lmel.size(); i++) {
	sprintf(buf, "LM%d", i);
	lmel[i] = atof(getResult(buf).c_str());
	sprintf(buf, "RM%d", i);
	rmel[i] = atof(getResult(buf).c_str());
      }
    }
    //if (cl("-spec").optionarg=="lpc") {
    //  pds.getLPCSpectra(this);
    //}
  }
}


BOSS_NodeDE::~BOSS_NodeDE() {}

BOSS_NodeDE & BOSS_NodeDE::operator = (const BOSS_NodeDE & b)
{
	// cl missing?
	unit_node = b.unit_node;
	user_data = b.user_data;
	type      = b.type;
	pos       = b.pos;
	cost      = b.cost;
	pred      = b.pred;
	Num       = b.Num;
	tkey	  = b.tkey;
	treal	  = b.treal;
	cleft     = b.cleft;
	cright    = b.cright;
	pmode     = b.pmode;
	pint	  = b.pint;
	first     = b.first;
	last      = b.last;
	stress	  = b.stress;
	lf0       = b.lf0;
	rf0       = b.rf0;
	avgf0     = b.avgf0;
	rms       = b.rms;
	lmel      = b.lmel;
	rmel      = b.rmel;
	return *this;
}

/** @} */ //end of boss_unitselection group

