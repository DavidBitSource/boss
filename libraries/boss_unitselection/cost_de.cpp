/* cost_de.cpp    $Revision: 1.3 $ $Date: 2006/04/08 16:45:27 $
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
 * @file cost_de.cpp
   @author Karlheinz Stoeber
   @author Stefan Breuer
   @author Joerg Broeggelwirth
   @author Michal Holcik
*/

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include "boss_utility/dom_tools.h"
#include "cost_de.h"
#include "boss_node_de.h"

/** @addtogroup boss_unitselection BOSS UnitSelection Module
 * @{
 */

/// @brief CostDE constructor
/// @param __cl system wide config container
CostDE::CostDE(BOSS::Config & __cl): Cost(__cl) {}
/// @brief CostDE destructor
CostDE::~CostDE() {}


/**
 * @brief Decides whether the argument is sibilant or not.
 * @param s input string
 * @return true iff the input is a sibilant, false otherwise
 */
bool CostDE::isSibilant(const string & s) const {
  if (s == "s" || s == "z" || s == "S" || s == "tS" || s == "ts" || s == "dZ" || s == "Z") return true;
  return false;
}

/**
 * @brief Decides whether or not the argument is a nasal.
 * @param s input string
 * @return true iff the input is a nasal, false otherwise
 */
bool CostDE::isNasal(const string & s) const {
  if (s == "n" || s == "m" || s == "N" || s == "@n" || s == "@m" || s == "@N") return true;
  return false;
}


/**
 * @brief Computes the cost of stress parameteres of a node.
 * @param node a node whose stress cost is to be computed
 * @return stress cost of a node
 */
double CostDE::stress_cost(const BOSS_Node * node) const {
  double cost(0);
  const BOSS_NodeDE * u  = dynamic_cast<const BOSS_NodeDE*>(node);
  if (u->type != BOSS_Node::WORD) {
    unsigned pstress=translateStress(atoi(BOSS::getAttrByName(u->unit_node, "Stress").c_str()));
    unsigned rstress=translateStress(u->stress);
    cost = fabs(double(pstress)-double(rstress));
    if (cl("-v").optionflag) 
      cerr << "predicted Stress: " << pstress
	   << " real Stress: " << rstress
	   << " unweighted cost: " << cost;
  }
  return cost;
}

/**
 * @brief Computes the cost of phrasing of a node.
 * @param node  a node whose phrasing cost is to be computed
 * @return phrasing cost of a node
 */
double CostDE::phrasing_cost(const BOSS_Node * node) const {
  double cost(0);
  const BOSS_NodeDE * u  = dynamic_cast<const BOSS_NodeDE*>(node);
  if (u->type != BOSS_Node::WORD) {
    string pmode = BOSS::getAttrByName(u->unit_node, "PMode").c_str();
    int pint = atoi(BOSS::getAttrByName(u->unit_node, "PInt").c_str());
    if ((pmode == "")&&(u->pmode!="")) cost+=2.0;
    if ((pmode == "?")&&(u->pmode!="?")) cost+=1.0;
    if ((pmode == ".")&&(u->pmode!=".")) cost+=1.0;
    if (cl("-v").optionflag) 
      cerr << "predicted PMode/PInt: " << pmode << "/" << pint
	   << " - real PMode/PInt: " << u->pmode << "/" << u->pint
	   << " unweighted cost: " << cost;
  }
  return cost;
}

/**
 * @brief Computes a duration cost of a node.
 * @param node a node whose duration cost is to be computed
 * @return duration cost of a node
 */
double CostDE::duration_cost(const BOSS_Node * node) const {
  const BOSS_NodeDE * u  = dynamic_cast<const BOSS_NodeDE*>(node);
  double pdur = atof(cl("-fac_dur").optionarg.c_str())*(double(atoi(BOSS::getAttrByName(u->unit_node, "Dur").c_str())));
  double rdur = double(u->last-u->first)/16.0;
  double ddur = rdur-pdur;
  double cost = fabs(ddur);
  // normalization of the duration differences on different unit levels
  if (u->type != BOSS_Node::PHONE && u->type != BOSS_Node::HALFPHONE) { // code changed for halfphone adaptation
    cost /= atoi(BOSS::getAttrByName(u->unit_node, "PNr").c_str());
  }
  if (cl("-v").optionflag) 
    cerr << BOSS::getAttrByName(u->unit_node, "Phrpos")
	 << " pred: " << pdur << " real: " << int(rdur+.5)
	 << " diff: " << int(ddur+.5)
	 << " unweighted cost: " << cost;
  return cost;
}

/**
 * @brief Assigns costs to discrepancies between the canonical transcription (TKey) and the actual realisation (TReal) of a unit.
 * @param node a node
 * @return Cost of discrepancies between the canonical transcription (TKey) and the actual realisation (TReal) of a unit.
 */
double CostDE::treal_cost(const BOSS_Node * node) const {
  const BOSS_NodeDE * u  = dynamic_cast<const BOSS_NodeDE*>(node);
  double c1 = 100.0;
  double c2 = 200.0;
  //	double c3 = 300.0;
  //	double c4 = 400.0;
  double c5 = 500.0;
  double cost = 0.0;
  if (u->type == BOSS_Node::PHONE && u->tkey != u->treal) {
    vector<string> treal;
    treal.push_back(string(1,u->treal[0]));
    bool flag=false;
    for (unsigned i=1;i<u->treal.size();i++) {
      if (u->treal[i]=='_') flag=true;
      else {
	if (flag) {
	  treal.push_back(string(1,u->treal[i]));
	  flag=false;
	}
	else treal[0]=treal[0]+u->treal[i];
      }
    }
    if (treal[0] != u->tkey) return c5;
    for (unsigned i=1;i<treal.size();i++) {
      if (treal[i]=="n") {
	if ((treal[0] == "t" || treal[0] == "b") && u->cright == "@n") cost += c1;
	else cost += c2;
	continue;
      }
      if (treal[i]=="\\") {
	if (treal[0] == "t" && isSibilant(u->cright) && isSibilant(u->cleft)) cost += c1;
	else cost += c2;
	continue;
      }
      if (treal[i] == "0") {
	if (treal[0] != "d" || treal[0] != "b" || treal[0] != "g" || treal[0] != "z") cost += c2;
	continue;
      }
      if (treal[i] == "v") {
	cost += c2;
	continue;
      }
      if (treal[i] == "~") {
	if (isNasal(u->cleft) && isNasal(u->cright)) cost += c1;
	else cost += c2;
	continue;
      }
    }			
    if (cl("-v").optionflag) 
      cerr << " unweighted cost: " << cost;
  }
  return cost;
} 


/**
 * @brief Computes the total cost of a node
 * @param node a node whose total cost is to be computed
 * @return total cost of a node
 */
double CostDE::operator () (const BOSS_Node * node) const
{
  double cost(0),div(0),tmp(0);
  const BOSS_NodeDE * u  = dynamic_cast<const BOSS_NodeDE*>(node);
  if(u->type != BOSS_Node::INITIAL && u->type != BOSS_Node::FINAL) {
    // log information
    if (cl("-v").optionflag) 
      cerr << "UNIT cost: " << u->Num << " "
	   << BOSS::getAttrByName(u->unit_node, "TKey") << " (" 
	   << u->treal << ")" << endl;
    if (atof(cl("-ucw_dur").optionarg.c_str())>0.0) {
      div+=1.0;
      if (cl("-v").optionflag) cerr << "   duration: ";
      tmp = atof(cl("-ucw_dur").optionarg.c_str())*duration_cost(u);
      cost += tmp;
      if (cl("-v").optionflag) cerr << " weighted cost: " << tmp << endl;
    }
    if (atof(cl("-ucw_stress").optionarg.c_str())>0.0) {
      div+=1.0;
      if (cl("-v").optionflag) cerr << "   stress: ";
      tmp = atof(cl("-ucw_stress").optionarg.c_str())*stress_cost(u);
      cost += tmp;
      if (cl("-v").optionflag) cerr << " weighted cost: " << tmp << endl;
    }
    if (atof(cl("-ucw_phrase").optionarg.c_str())>0.0) {
      div+=1.0;
      if (cl("-v").optionflag) cerr << "   phrasing: ";
      tmp = atof(cl("-ucw_phrase").optionarg.c_str())*phrasing_cost(u);
      cost += tmp;
      if (cl("-v").optionflag) cerr << " weighted cost: " << tmp << endl;
    }
    if (atof(cl("-ucw_treal").optionarg.c_str())>0.0) {
      div+=1.0;
      if (cl("-v").optionflag) cerr << "   treal: ";
      tmp = atof(cl("-ucw_treal").optionarg.c_str())*treal_cost(u);
      cost += tmp;
      if (cl("-v").optionflag) cerr << " weighted cost: " << tmp << endl;
    }
    if (div>1.0)
      cost/=div;
    if(cl("-v").optionflag)
      cerr << "TOTAL UNIT cost:  " << cost
	   << ", using " << div << " unit cost function(s)." << endl;
  }
  return cost;
}

/**
 * @brief Computes the Euclidean distance between the mel frequency cepstrum coefficients of two nodes.
 * @param l_node first node
 * @param r_node second node
 * @return Euclidean distance between the mel frequency cepstrum coefficients of two nodes.
 */
float CostDE::mel_dist (const BOSS_Node * l_node, const BOSS_Node * r_node) const
{
  const BOSS_NodeDE * l = dynamic_cast<const BOSS_NodeDE*>(l_node),
                    * r = dynamic_cast<const BOSS_NodeDE*>(r_node);
  if(l->realNode() && r->realNode()) {
    return euclid(l->rmel, r->lmel);
  }	
  return 0;
}


//   float CostDE::lpc_dist (const BOSS_Node & l, const BOSS_Node & r) const
//   {
//   if(l.realNode() && r.realNode()) {
//   double cost(0);
//   // 1. Calculation of KL-distance:
//   for(int i=0;i<=defaultWindowSize/2;i++)
//   {
//   if ((r.pds.spectruml[i]!=0.0) && (l.pds.spectrumr[i]!=0.0))
//   cost = cost+(l.pds.spectrumr[i]-r.pds.spectruml[i])*log(l.pds.spectrumr[i]/r.pds.spectruml[i]);
//   }
//   return (float)cost;
//   }	
//   return 0; // 1.0 in IPO implementation...
//   }


/**
 * @brief Computes fundamental frequency distance of two nodes.
 * @param l_node first node
 * @param r_node second node
 * @return F0 distance of two nodes
 */ 
float CostDE::f0_dist (const BOSS_Node * l_node, const BOSS_Node * r_node) const
{
  const BOSS_NodeDE * l = dynamic_cast<const BOSS_NodeDE*>(l_node),
                     * r = dynamic_cast<const BOSS_NodeDE*>(r_node);
  if(l->realNode() && r->realNode())
    if (l->getNodeType() == BOSS_Node::PHONE && r->getNodeType() == BOSS_Node::PHONE)
      if (l->rf0!=0 && r->lf0!=0)
	return fabs(l->rf0-r->lf0);
  return 0;
}

/**
 * @brief Computes total cost of transition between two nodes.
 * @param l_node first node
 * @param r_node second node
 * @return total transition cost of two nodes
 */ 
float CostDE::operator () (const BOSS_Node * l_node, const BOSS_Node * r_node)
{
  const BOSS_NodeDE * l = dynamic_cast<const BOSS_NodeDE*>(l_node),
                    * r = dynamic_cast<const BOSS_NodeDE*>(r_node);
  float cost(0),div(0);
  if (cl("-v").optionflag)
    cerr << "TRANSITION cost: "
	 << l->Num << "->"
	 << r->Num << " "
	 << l->tkey << "->"
	 << r->tkey << endl;
  if (cl("-spec").optionarg=="melc")  
    if (atof(cl("-tcw_melc").optionarg.c_str())>0.0) {
      div+=1.0;
      cost += atof(cl("-tcw_melc").optionarg.c_str())*mel_dist(l,r);
    }
//   if (cl("-spec").optionarg=="lpc")
//     if (atof(cl("-tcw_lpc").optionarg.c_str())>0.0) {
//       div+=1.0;
//       cost += atof(cl("-tcw_lpc").optionarg.c_str())*lpc_dist(l,r);
//     }
  if (atof(cl("-tcw_f0").optionarg.c_str())>0.0) {
    div+=1.0;
    cost += atof(cl("-tcw_f0").optionarg.c_str())*f0_dist(l,r);
  }
  if (div>1.0)
    cost/=div;
  if (cl("-v").optionflag) cerr << "TOTAL TRANSITION cost:  " << cost << ", using " << div << " transition cost function(s)." << endl;
  return cost;
}

/** @} */ //end of boss_unitselection group

