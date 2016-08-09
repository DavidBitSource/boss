/* boss_concat.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_concat.cpp
 * @author Stefan Breuer
 */

#include "boss_concat.h"
#include <math.h>

/**
 * @addtogroup boss_concat
 * @{
 */

BOSS_Concat::BOSS_Concat(BOSS::Config & __cl, MySQL_Handler * __data_base): BOSS_ConMan(__cl, __data_base) {
  cerr << "Construction of class Concat ... ";
  sflag = (!__cl("-no_smooth").optionflag);
  cerr << "OK" << endl;
}

void BOSS_Concat::getCM_Units(xercesc::DOMNode* node, CU_Vector & cv) {
  if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
    DOM_UserData * user_data;	
    if((user_data = (DOM_UserData*)BOSS::getUserData(node)) != 0) {
      if(user_data->getPos() != DOM_UserData::npos) {
	string tkey = BOSS::getAttrByName(node, "TKey");
	if (tkey!=BOSS_PAUSE_SYMBOL) {
	  cv.push_back(CM_Unit(cl, data_base, node, tkey, cl("-raw_path").optionarg, cl.getInvName(), cl("-raw_ext").optionarg));
	}
	else cv.push_back(CM_Unit(cl, data_base, node, tkey));
      }
    }
    else {
      xercesc::DOMNodeList* nl = node->getChildNodes();
      for(unsigned i=0; i<nl->getLength(); i++) {
	xercesc::DOMNode* tmp = nl->item(i);
	if(tmp->getNodeType() == xercesc::DOMNode::ELEMENT_NODE)
	  getCM_Units(tmp,cv);
      }
    }
  }
}

void BOSS_Concat::smoothing(CM_Unit & ul, CM_Unit & ur, t_Signal & res, t_Signal add) {
  cerr << "---Smoothing " << ul.tkey << " - " <<  ur.tkey << endl;
  int dist(SMOODIST);
  while (dist*2 > floor((float)res.size()/2) ||
	 dist*2 > floor((float)add.size()/2) ||
	 dist >= LEFTRIGHTOL) dist--;
  if (dist<2) {
    res.cat(add);
    return;
  }
  unsigned start=res.size()-dist;
  t_Signal tmp(dist);
  res.cat(tmp);
  tmp.cat(add);
  add=tmp;
  //dist = 1/4 of a window
  unsigned i(0),j(0);
  for (i=ur.start-dist;i<ur.start;i++) 
    add[j++]=int(ur.signal[i]*dsp::hanning(j,dist*4));
  for (i=unsigned(dist);i<unsigned(dist*2);i++)
    add[i]=int(add[i]*dsp::hanning(j++,dist*4));
  for (i=(res.size()-1)-2*dist;i<(res.size()-1)-dist;i++)
    res[i]=int(res[i]*dsp::hanning(j++,dist*4));
  for (unsigned k=ul.end+1;k<=ul.end+dist;k++) {
    res[i++]=int(ul.signal[k]*dsp::hanning(j++,dist*4));
  }
  res.cat(t_Signal(add.size()-dist*2));
  i=0;
  for (unsigned l=start;l<res.size();l++)
    res[l]=res[l]+add[i++];
}

void BOSS_Concat::preEmph(t_Signal & res) {
  //Apply pre-emphasis to signal
  float p1 = atof(cl("-p1").optionarg.c_str());
  float p2 = atof(cl("-p2").optionarg.c_str());
  if(p1 != 0.0) {
    for(unsigned i=1; i<res.size(); i++)
      res[i-1] = short(rint(float(res[i-1]) - p1*float(res[i])));
  }
  if (p2 != 0.0) {
    for(unsigned i=1; i<res.size(); i++)
      res[i-1] = short(rint(float(res[i-1]) - p2*float(res[i])));
  }
}


string BOSS_Concat::getDescription() {
	return "BOSS_Concat Module Description: Signal concatenation module.";
}

void BOSS_Concat::operator () (xercesc::DOMNode* sentence)
{
  t_Signal res;
  CU_Vector cv;
  getCM_Units(sentence, cv);
  if (!cv.size()) return;
  unsigned size(0);
#ifndef NOTVERB
  t_Signal sum;
  stringstream filebase;
  filebase << string(BOSS::getAttrByName(sentence,"TargetPath"))
	   << "/" 
	   << string(BOSS::getAttrByName(sentence,"TargetFile"))
	   << "_"
	   << BOSS::getAttrByName(sentence,"SentenceID");
  ofstream out(string(filebase.str()+"_plain.raw").c_str());
  ofstream blf(string(filebase.str()+"_plain.blf").c_str());
  for (unsigned i=0;i<cv.size();i++) {
    if (cv[i].pause) sum.cat(cv[i].signal);
    else
      sum.cat(t_Signal(cv[i].signal,cv[i].start,cv[i].end));
    blf << size << " " << cv[i].tkey << endl;
    size=sum.size();
  }
  sum.write(out);
  out.close();
  blf.close();
  size=0;
#endif
  for (unsigned i=0;i<cv.size();i++) {
    cerr << endl << "+++ Concatenation UNIT "  << cv[i].tkey << endl;
    if (cv[i].pause) res.cat(cv[i].signal);
    else {
      if (sflag && i>0 && !cv[i-1].pause)
	smoothing(cv[i-1],cv[i],res,t_Signal(cv[i].signal,cv[i].start,cv[i].end));
      else res.cat(t_Signal(cv[i].signal,cv[i].start,cv[i].end));
    }
    cv[i].user_data->setSignalSize(res.size()-size);
    size=res.size();
  }
  preEmph(res);
  writeSignalToXML(res, sentence);
}

/** @} */ // end of boss_concat group
