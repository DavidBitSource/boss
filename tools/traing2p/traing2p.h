/* traing2p.h    $Revision: 1.3 $ $Date: 2005/12/10 20:33:54 $
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

#ifndef TRAING2P_H
#define TRAING2P_H


#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include "boss_g2p/boss_g2p.h"
#include "boss_g2pmatrix.h"
#include "phimbl/phimbl.h"


/**
 *@author Stefan Breuer
 */

#define MAX_NUMBER_OF_ALIGNMENTS 1000

struct t_MappingTableEntry {
  string key;
  string target;
};

inline istream & operator >> (istream &in, t_MappingTableEntry & mte)
{ return in >> mte.key >> mte.target; }

typedef map< string, string > t_MappingTableMap;
typedef enum {G2P, SYLL} TrainingType;

struct t_Alignment {
  string word;
  BOSS::t_SVector graph;
  vector<BOSS::t_SVector> phon;
  BOSS::t_SVector best;
  bool accepted;
  void size() { cerr << graph.size() << " " << phon.size() << " " << best.size() << endl; }
};
//typedef vector<t_Alignment> t_AlVector;

class Traing2p : public BOSS_g2p {
 public:
  Traing2p(BOSS::Config & config) : BOSS_g2p(config) {}
  Traing2p(const BOSS_g2p & c) : BOSS_g2p(c) {}
  Traing2p(BOSS::Config & config, const string & traintype, const bool & createflag, const string & phonfile, const string & treefile, const string & lexfile, const string & featurefile, const LabelFormat & F);
  
 protected:
  void trainphon(const string & lexfile, const string & treefile, const string & featurefile, const LabelFormat & F);
  void trainsyll(const string & lexfile, const string & treefile, const string & featurefile, const LabelFormat & F);
  void trainstress(const string & lexfile, const string & treefile, const string & featurefile, const LabelFormat & F);
  void create_tree(const string & featurefile, const string & treefile);
  string clean_blftrans(const string & s);
  string clean_blftrans(const string & s, bool syll_flag);
  string remove_quotes(const string & s);
  BOSS::t_SVector extract_graphemes(t_MappingTableMap & mt);
  t_Alignment align_pair(const string & graph, const string & phon, BOSS_g2pMatrix & gpm);
  void assign_scores(const t_Alignment & al, BOSS_g2pMatrix & gpm);
  t_Alignment find_bestAlignment(t_Alignment al, BOSS_g2pMatrix & gpm);
  BOSS::t_SVector generateSyllFeatures(const BOSS::t_SVector & t);
  BOSS::t_SVector generateStressFeatures(const BOSS::t_SVector & t);
  BOSS::t_SVector generate_trainset(BOSS_g2pMatrix & gpm);
  void print_matrix(BOSS_g2pMatrix & m);
  double binomial(const double & n, const double k);
  BOSS::Phimbl phimbl;
};

#endif

