/* boss_morphemes_de.cpp    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
   @author Karlheinz Stoeber
   @author Joerg Broeggelwirth
*/

#include <sstream>
#include <algorithm>
#include <set>
#include <iterator>
#include "boss_morphemes_de.h"
#include "boss_morphtree.h"
#ifdef BONT
 #include <fstream>
#else
 #include "boss_mysql/boss_mysql_handler.h"
 #include "boss_mysql/boss_mysql_result.h"
#endif
#include "boss_utility/boss_config.h"

/**
 * @addtogroup boss_transcription
 * @{
 */
#ifndef BONT 

#else
extern p_Vector p_table;
m_Vector m_table;

static const char morphemelexiconfile[] = "./data/runtime/morpheme_lexicon.txt";
static const char phonesfile[] = "./data/runtime/phones.txt";
#endif

// ****************************************************************************
// ****************************************************************************

BOSS_MorphemeLexicon_DE::BOSS_MorphemeLexicon_DE(BOSS::Config & __cl, MySQL_Handler * __data_base): cl(__cl), data_base(__data_base)
{
#ifndef BONT
  stringstream query;
  query << "SELECT Orth FROM morpheme_lexicon" << ends;
  MySQL_Result res;
  try {
    res = data_base->query(query.str().c_str());
  }
  catch(const MySQL_QueryFailed & e) {
  	e.printError(true);
  }

  for(unsigned i=0; i<res.row(); i++) {
    const string & tmp = res(i, 0);
    lexicon_map[tmp[0]].push_back(tmp);
  }
#else
  // Reading of the morpheme lexicon
  ifstream in(morphemelexiconfile);
  if (in) in >> m_table;
  else cerr << "Can't open morpheme lexicon file!" << endl;

  for(unsigned i=0; i<m_table.size(); i++) {
    const string & tmp = m_table[i].orth;
    //cerr << "BOSS_MorphemeLexicon_DE: tmp: " << tmp << endl;
    lexicon_map[tmp[0]].push_back(tmp);
  }
#endif
}

// ****************************************************************************
// ****************************************************************************

t_SubStringEntryVector BOSS_MorphemeLexicon_DE::find(const string & word) const
{
 t_SubStringEntryVector res;
  unsigned pos;
  t_LexiconMapCIt it;
  bool found;
  for(unsigned i=0; i<word.size(); i++) {
    if((it = lexicon_map.find(word[i])) != lexicon_map.end()) {
      const BOSS::t_SVector & tmp = (*it).second;
      for(unsigned j=0; j<tmp.size(); j++) {
	if(((pos = word.find(tmp[j], i)) != string::npos) && pos == i)
	{
	  found = false;
	  for (t_SubStringEntryVector::iterator i1 = res.begin(); i1 != res.end(); i1++)
	  {
	    if ((*(*i1).morph == tmp[j]) && ((*i1).left == pos)) 
	    {
	      found = true;
	      break;
	    }
	  }
	  if (!found) res.push_back(t_SubStringEntry(pos, pos + tmp[j].size(), &tmp[j]));
	}
      }
    }
  }
  // sort(res.begin(), res.end());
  return res; 
}

// ****************************************************************************
// ****************************************************************************

BOSS_Morphemes_DE::BOSS_Morphemes_DE(BOSS::Config & __cl, MySQL_Handler * __data_base): cl(__cl), data_base(__data_base), lexicon(__cl, __data_base)
{

  try {
  #ifndef BONT 
       pfsa = new BOSS::FSA(cl("-morphsym").optionarg);
  #else
       pfsa = new BOSS::FSA(phonesfile);
  #endif
  }
  catch(const BOSS::FileOpenError & e) {
    e.printError(true);
  }
  catch(const BOSS::FileFormatError & e) {
    e.printError(true);
  }
}

// ****************************************************************************
// ****************************************************************************

BOSS_Morphemes_DE::~BOSS_Morphemes_DE(){
  if (pfsa) delete pfsa;
}

// ****************************************************************************
// ****************************************************************************

bool isConnected(const t_SubStringEntryVector & v, const unsigned & i)
{
  set< unsigned > lset, rset;
  for(unsigned j=0; j<v.size(); j++) {
    if(j != i) {
      lset.insert(v[j].left);
      rset.insert(v[j].right);
    }
  }
	
  if(lset.find(v[i].right)!=lset.end() && rset.find(v[i].left)!=rset.end())
    return true;
		
  return false;
}

// ****************************************************************************
// ****************************************************************************

t_SSPairVector BOSS_Morphemes_DE::selectLong(t_SubStringEntryDList & slist) const
{
  t_SubStringEntryDList::iterator it;
  t_SubStringEntryList::iterator itl;
  unsigned i(0), j, min0(unsigned(-1)), min1(unsigned(-1)), minp(0), p0, p1;
  for(it=slist.begin(); it!=slist.end(); it++, i++) {
    for(j=p0=p1=1, itl=(*it).begin(); itl!=(*it).end(); itl++, j++) {
      p0 *= (*itl).morph->size();
      p1 *= j * (*itl).morph->size();
    }
    if(p0<=min0) {
      if(p1<=min1) {
	min0 = p0;
	min1 = p1;
	minp = i;
      }
    }
  }

  it = slist.begin();
  advance(it, minp);
	
  t_SSPairVector result;
  list< t_SubStringEntry >::const_iterator itres;
#ifdef BONT
    m_Vector tmp;
#endif
  for(itres = (*it).begin(); itres != (*it).end(); itres++) {
#ifndef BONT
    stringstream query;
    //cerr << "SELECT Trans,Class FROM morpheme_lexicon WHERE Orth = \"" << *(*itres).morph << "\""<< endl;
    query << "SELECT Trans,Class FROM morpheme_lexicon WHERE Orth = \"" << *(*itres).morph << "\"" << ends;
    MySQL_Result res;
    try {
      res = data_base->query(query.str().c_str());
    }
    catch(const MySQL_QueryFailed & e) {
      e.printError(true);
    }
#else
    for (unsigned i=0; i<m_table.size(); i++) {
      if (m_table[i].orth==*(*itres).morph) tmp.push_back(m_table[i]);
    }
#endif
  
    // Joerg: no endings at the beginning of a word!!!
#ifndef BONT
    //cerr << "database query res: " << res(1, 0) << endl;
    //for(unsigned i=0; i<min(res.row(), unsigned(1)); i++)
    bool res_pushed(false);
    for (unsigned i=0; i<res.row(); i++) {
      //cerr << "res.row(): " << res.row() << " res(i, 0): " << res(i, 0) << endl;
      if (!res_pushed) {
	if ((itres==it->begin())&&(res(i, 1)!="e")) {
	  result.push_back(BOSS::t_SSPair(res(i, 0), res(i, 1)));
	  //cerr << "pushed res: " << res(i,0) << " " << res(i, 1) << endl;
	  res_pushed=true;
	}
	else if (i==res.row()-1) {
	  result.push_back(BOSS::t_SSPair(res(0, 0), res(0, 1)));
	  //cerr << "pushed res: " << res(0,0) << " " << res(0,1) << endl;
	  res_pushed=true;
	}
      }
    }
  } 
#else
 bool res_pushed(false);
    for (unsigned j=0; j<tmp.size(); j++) {
      if (!res_pushed) {
	if ((itres==it->begin())&&(tmp[j].m_class!="e")||(j==tmp.size()-1)) {
	  result.push_back(BOSS::t_SSPair(tmp[j].trans, tmp[j].m_class));
	  res_pushed=true;
	}
      }
    }
    tmp.clear();
  } 
#endif
  //	copy((*it).begin(), (*it).end(), ostream_iterator< t_SubStringEntry >(cout, " "));
  return result;
}

// ****************************************************************************
// Joerg
// ****************************************************************************

bool BOSS_Morphemes_DE::is_vowel(const string & phone) const
{
#ifndef BONT
  MySQL_Result res;		
  stringstream query;
  query << "SELECT Class FROM phone_table WHERE Phon=\"" << phone << "\"" << ends;
  try {
    res = data_base->query(query.str().c_str());
  }
  catch(const MySQL_QueryFailed & e) {
    e.printError(true);
  }
  if (res.row()>0&&(res(0,0)=="dt"||res(0,0)=="lv"||res(0,0)=="sv")) return true;
#else
  struct p_line tmp;
  for (unsigned i=0; i<p_table.size(); i++) {
    if (p_table[i].phon==phone) tmp=p_table[i];
  }
  if (tmp.phon!=""&&tmp.p_class=="dt"||tmp.p_class=="lv"||tmp.p_class=="sv") return true;
#endif
  else return false;
}

// ****************************************************************************
// ****************************************************************************

void BOSS_Morphemes_DE::maximum_onset(BOSS::t_SVector & coda, t_SDeque & onset) const
{
  // Definition of valid german onsets consisting of more than 1 phone
  const unsigned k=48;
  static const string help[k] = {"pf", "ps", "pr", "pR", "pl", "bl", "br", "bR", "ts", "tS",
				 "tr", "tR", "dZ", "dr", "dR", "gn", "gr", "gR", "gl", "kn",
				 "kv", "kr", "kR", "kl", "fr", "fR", "fl", "sk", "Sp", "St",
				 "Sm", "Sn", "Sv", "Sr", "SR", "Sl", "pfl", "pfr", "pfR",
				 "tsv", "skr", "skR", "skl", "Spr", "SpR", "Spl", "Str", "StR"};
  set<string> valid_onset;
  valid_onset.insert(&help[0], &help[k]);
  string tmp("");
  while (onset.size()>1) {
    for (unsigned c=0; c<onset.size(); c++) tmp+=onset[c];
    if (valid_onset.count(tmp)==0) {
      coda.push_back(onset.front());
      onset.pop_front();
    }
    else break;
    tmp="";
  }
}

// ****************************************************************************
// ****************************************************************************

BOSS::t_SVector BOSS_Morphemes_DE::try_parse(const string & phones) const
{
  BOSS::t_SVector result;
  //cerr << "Parsing of: " << phones << endl;
  try {
    result = pfsa->parse(phones);
  }
  catch(const BOSS::NoRegexMatch & e) {
    e.printError(true);
  }
  return result;
}

// ****************************************************************************
// ****************************************************************************

t_SSPairVector BOSS_Morphemes_DE::join_stems(const t_SSPairVector & trans) const
{
  t_SSPairVector result;
  BOSS::t_SSPair left_pair, right_pair;
  if (trans.size()>1)
  {
    for (t_SSPairVector::const_iterator it=trans.begin(); it<(trans.end()-1); it++)
    {
      if (it==trans.begin()) {
	left_pair=*it;
	right_pair=*(it+1);
      }
      else right_pair=*(it+1);
      // Building of pseudo-stems
      /*if ((left_pair.second=="s"||left_pair.second=="p")&&(right_pair.second=="f")) {
	left_pair.first+=right_pair.first;
	left_pair.second="s";
	right_pair=left_pair;
	}*/
      /* else */ if ((left_pair.second=="s"||left_pair.second=="p")&&(right_pair.second=="s"||right_pair.second=="d"||
		right_pair.second=="p")) {    
	left_pair.first+=";"+right_pair.first;
	left_pair.second="s";
	right_pair=left_pair;
      }
      else {
	result.push_back(left_pair);
	left_pair=right_pair;
      }
    }
    result.push_back(right_pair);
  }
  else result=trans;
  return result;
}

// ****************************************************************************
// Function modified by PB.
// ****************************************************************************

BOSS::t_SVector BOSS_Morphemes_DE::syllable_border(const t_SSPairVector & joined_trans) const
{
  string tmp("");
  t_SDeque onset;
  BOSS::t_SVector left, right, coda, result;
  bool set_point(true);

  // if the morpheme string consists of only 1 morpheme, it is parsed and returned (as 'result')

  if (joined_trans.size()==1) { result = try_parse(joined_trans[0].first); }

  // ****************************************************************
  // build a new morpheme pair in each iteration
  // ****************************************************************

  for (t_SSPairVector::const_iterator it = joined_trans.begin(); it < (joined_trans.end()-1); it++) 
  {
    onset.clear();
    coda.clear();

    // build the morph-pair of the first iteration ******************
    if (it==joined_trans.begin()) 
    {
      // phone-parsing of the left and right morpheme
      left = try_parse(it->first);
      right = try_parse((it+1)->first);
    }

    // 'left' is already set and 'right' is taken from the morpheme-string
    else 
    {
      // phone-parsing of the actual right-morpheme
      right = try_parse((it+1)->first);
    }

    // find the next nucleus(vowel) left from the morpheme boundary		
    // This loop was modified by PB due to a runtime error with the original

    unsigned i=left.size();
    string lastElement;      // added by PB

    do 
    {
      i--;
      onset.push_front(left[i]);
      lastElement = left[i];
      left.pop_back();
    } while (i>0 && !is_vowel(lastElement));

    // if a nucleus(vocal) was found, shift it back from 'onset' to 'left'
    // This condition was modified by PB due to a runtime error with the original

    if (is_vowel(lastElement)) 
    {
      left.push_back(onset.front());
      onset.pop_front();
    }
    else 
      if (i==0) { set_point=false; }

    // find the next nucleus(vowel) right from the morpheme boundary
    unsigned j=0;
    do 
    {
      j++;
      onset.push_back(right[j-1]);
    } while (j<right.size() && !is_vowel(right[j-1]));

    // **************************************************************
    // check if a nucleus was found; otherwise set 'set_point' to false
    // (no syllable-boundary in final consonant clusters)
    // **************************************************************

    if (!is_vowel(right[j-1])) set_point=false;
    right.erase(right.begin(), right.begin()+(j-1));
    onset.pop_back();
    
    // Search for a valid 'onset', onsets consisting of only 1 phone are regarded as valid
    maximum_onset(coda, onset);
    
    // assign the phone-string with the correctly positioned point to 'result'
    
    if (set_point) 
    {
      result=result+left+coda+".";
    }
    else 
    {
      result=result+left+coda;
      set_point=true;
    }

    // **************************************************************
    // 'onset+right' is the new left part of the morph-pair
    // (the right-part is set at the beginning of the for-loop)
    // **************************************************************

    left=onset+right;
  }	// for-loop (t_SSPairVector-iterator 'it' is incremented)

  result=result+onset+right;

  return result;
}

// ****************************************************************************
// ****************************************************************************

BOSS::t_SVector BOSS_Morphemes_DE::morpheme_border(const t_SSPairVector & trans,
                                                const BOSS::t_SVector & syl_chain) const
{
  BOSS::t_SVector result, actual;
  string tmp("");
  // Conversion from t_SSPairVector to string (including symbol ',' for morpheme-boundaries)
  for (t_SSPairVector::const_iterator it=trans.begin(); it<(trans.end()-1); it++) {
    tmp+=it->first+",";	
  }
  tmp+=(trans.end()-1)->first;
  // phone-parsing of the tmp-string
  actual=try_parse(tmp);
  // Comparison of the morpheme-boundary vector ('actual') with the syllable-boundary
  // vector ('syl_chain'). A new vector containing both kinds of boundary information is created.
  unsigned i=0, j=0;
  while(i<actual.size() && j<syl_chain.size()) {
    if (actual[i]==syl_chain[j]) {
      result.push_back(actual[i]);
      i++;
      j++;
    }
    else {
      if (actual[i]==",") {
	if (syl_chain[j]==".") {
	  result.push_back(";");
	  i++;
	  j++;
	}
	else {
	  result.push_back(",");
	  i++;
	}
      }
      if (syl_chain[j]==".") {
	result.push_back(".");
	j++;
      }
    } // else
  } // while
  return result;
}

// ****************************************************************************
// ****************************************************************************

BOSS::t_SVector BOSS_Morphemes_DE::apply_morphonology(const t_SSPairVector & trans,
																						       const BOSS::t_SVector & final_trans) const
{
  // Definition of voiced obstruents which might be affected by "Auslautverhärtung"
  // (if 'g' occurs in 'Ig', it is changed to 'C')
  t_SSMap v_u_obstruent;
  v_u_obstruent["b"]="p";
  v_u_obstruent["d"]="t";
  v_u_obstruent["g"]="k";
  v_u_obstruent["v"]="f";
  v_u_obstruent["z"]="s";
  
  BOSS::t_SVector result(final_trans);
  t_SSPairVector::const_iterator it = trans.begin();
  
  for (unsigned i=0; i < result.size(); i++) 
  {
    // "Auslautverhärtung" in final position of the word ************
    if (i==(result.size()-1)&&v_u_obstruent.find(result[i])!=v_u_obstruent.end()) 
    {
      if (i>0&&(result[i-1]+result[i])=="Ig") result[i]="C";
      else result[i]=v_u_obstruent[result[i]];
    }

    // "Auslautverhärtung" before syllable-boundary *****************
    if ((result[i]=="."||result[i]==";")&&v_u_obstruent.find(result[i-1])!=v_u_obstruent.end()) {
      if (i>1&&(result[i-2]+result[i-1])=="Ig") result[i-1]="C";
      else result[i-1]=v_u_obstruent[result[i-1]];
    }
    // "Auslautverhärtung" before morpheme-boundary if the next phone isn't a vowel
    if (result[i]=="," && !is_vowel(result[i+1])&&v_u_obstruent.find(result[i-1]) != v_u_obstruent.end()) 
    {
      if (i>1&&(result[i-2]+result[i-1])=="Ig") result[i-1]="C";
      else result[i-1]=v_u_obstruent[result[i-1]];
    }

    // Treatment of 'IC' and '6' before morpheme-boundaries (not if two stems follow each other)
    if ((result[i]=="," || result[i]==";") && !(it->second=="s" && (it+1)->second=="s")) 
    {
      // change of 'C' to 'g' in morpheme 'IC' (-ig) before vowels (not for stems)
      if (is_vowel(result[i+1]) && it->first=="IC" && it->second!="s") result[i-1]="g";
      
      // change of '6' to 'r' inbetween two vowels and to '@''r' if preceded by a consonant
      // and followed by a vowel (rule not valid for prefix+(stem|prefix))

      if (it->second != "p" && i>1 && result[i-1]=="6") 
      {
	      switch (is_vowel(result[i+1])) 
        {
	        case true : 
            if (is_vowel(result[i-2])) 
            {
	            result[i-1]="r";
			        result[i]=",";
			        result.insert((result.begin()+(i-1)), ".");
            }
            else 
            {
			        result[i-1]="r";
			        result[i]=",";
			        result.insert((result.begin()+(i-1)), ".");
		          result.insert((result.begin()+(i-1)), "@");
  		      } 
          break;
      	} // switch
      }
    } // If ("," ";")

    // insertion of glottal stops before vowels at the beginnning of a word or stem
    if (i==0&&is_vowel(result[i])) 
    {
      reverse(result.begin(), result.end());
      result.push_back("?");
      reverse(result.begin(), result.end());
    }

    if (result[i]==";" && is_vowel(result[i+1]) && ((it+1)->second == "s" || (it+1)->second == "p"))
    {
      result.insert(result.begin()+(i+1), "?");
    }

    // Increment of trans-iterator
    if (result[i]==";"||result[i]==",") { it++; }
  }		

  return result;
}

// ****************************************************************************
// Entry point for this module.
// ****************************************************************************

string BOSS_Morphemes_DE::operator () (const string & word) const
{
  t_SubStringEntryVector matches = lexicon.find(word);
  matches.push_back(t_SubStringEntry(word.size(), 0));		
  t_SubStringEntryVector connected;
  bool change(true);

  while (change) 
  {
    change = false;
    for (unsigned i=0; i<matches.size(); i++) 
    {
      if(isConnected(matches, i) || matches[i].left == word.size())
      {
	      connected.push_back(matches[i]);
      }
      else
      {
	      change = true;
      }
    }
    matches = connected;
    connected.clear();
  }

  if (matches.size()==0) { return string(); }

  c_MorphTree mt(matches);
  t_SubStringEntryDList slist;

  mt.shortpath(slist);

  t_SSPairVector trans, joined_trans;
  BOSS::t_SVector final_trans, orth_morph;

  if(slist.size()) 
  {
    trans = selectLong(slist);

    joined_trans = join_stems(trans);

    final_trans=apply_morphonology(trans, morpheme_border(joined_trans, syllable_border(joined_trans)));
  }

  return t_SVector_to_string(final_trans);
}

// ****************************************************************************

/** @} */ // end of boss_transcription group
