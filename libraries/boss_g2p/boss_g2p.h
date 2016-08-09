/* boss_g2p.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/**
 * @file boss_g2p.h
   @author Stefan Breuer
   @author Philip Gross
*/

#ifndef BOSSG2P_H
#define BOSSG2P_H


#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <valarray>
#include "boss_utility/boss_fsa.h"
#include "boss_utility/boss_utility.h"
#include "boss_utility/boss_config.h"
#include "tdsp/dsp_matrix.h"
#include "phimbl/phimbl.h"

using namespace std;

// Classifier settings
#define TRANS_TRAIN_WINDOW 4
#define SYLL_TRAIN_WINDOW 4
#define STRESS_TRAIN_WINDOW 4

#define TIMBLCLASS_SYLLABLE_BOUNDARY_PRESENT "yes"
#define TIMBLCLASS_SYLLABLE_BOUNDARY_ABSENT "no"
#define TIMBLCLASS_PRIMARY_LEXICAL_STRESS_PRESENT "primary"
#define TIMBLCLASS_SECONDARY_LEXICAL_STRESS_PRESENT "secondary"
#define TIMBLCLASS_LEXICAL_STRESS_ABSENT "no"

// Symbols used in incoming and resulting transcriptions

#define NULL_PHONEME "<NULL_PHONEME>"
#define SYLLABLE_BOUNDARY_TAG "."
#define WORD_BOUNDARY_TAG "#"
#define PRIMARY_LEXICAL_STRESS_TAG "\""
#define SECONDARY_LEXICAL_STRESS_TAG "%"

typedef map< string, unsigned > t_GPTableMap;
typedef enum {BLF, HADIFIX} LabelFormat;

/**
 */
class BOSS_g2p {
 public:
  BOSS_g2p(BOSS::Config & __cl);
  BOSS_g2p(BOSS::Config & __cl, const LabelFormat & F);
  BOSS_g2p(const BOSS_g2p & c);
  virtual BOSS_g2p & operator = (const BOSS_g2p & c);
  virtual ~BOSS_g2p();
  string transcribe(const string & s) const;
  string g2p(const string & s) const;
  string assign_stress(const string & s) const;
  string syllabify(const string & s) const;
 protected:
  BOSS::Config & cl;
  BOSS::FSA * g2ppfsa;
  BOSS::FSA * syllpfsa;
  BOSS::FSA * stresspfsa;
  BOSS::t_SVector pv;
  void iniPointers();
  void delPointers();
  BOSS::t_SVector read_phoneset(const string & phonfile);
  vector< BOSS::t_SVector > generateG2PFeatures(BOSS::t_SVector g, BOSS::t_SVector p) const;
  BOSS::t_SVector string2SyllFeature(const BOSS::t_SVector & t,const unsigned & j) const;
  BOSS::t_SVector string2StressFeature(const BOSS::t_SVector & t,const unsigned & j) const;
  virtual string postProcPhon(const string & s) const { return s; }
  virtual string postProcSyll(const string & s) const { return s; }
  virtual string postProcStress(const string & s) const { return s; }
  virtual string postProcTrans(const string & s) const { return s; }
  BOSS::Phimbl phimbl;
  BOSS::Phimbl::t_Data g2pData;
  BOSS::Phimbl::t_Data syllData;
  BOSS::Phimbl::t_Data stressData;
};

#endif
