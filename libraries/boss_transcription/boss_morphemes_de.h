/* boss_morphemes_de.h    $Revision: 1.4 $ $Date: 2007/04/16 09:43:33 $
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
 * @file boss_morphemes_de.h
   @author Karlheinz Stoeber
   @author Joerg Broeggelwirth
*/

#ifndef BOSS_MORPHEMES_DE_H
#define BOSS_MORPHEMES_DE_H

#include <string>
#include <vector>
#include <map>
#include <list>
#include <deque>
#include "boss_utility/boss_types.h"
#include "boss_utility/boss_fsa.h"
#include "boss_utility/boss_config.h"

 #include "boss_mysql/boss_mysql_handler.h"
 #include "boss_mysql/boss_mysql_result.h"

using namespace std;


/**
 * @addtogroup boss_transcription
 * @{
 */

typedef vector< BOSS::t_SSPair >           t_SSPairVector;
typedef map< char, BOSS::t_SVector >       t_LexiconMap;
typedef t_LexiconMap::iterator       t_LexiconMapIt;
typedef t_LexiconMap::const_iterator t_LexiconMapCIt;

/**
  *@author Karlheinz Stoeber
  * A t_SubStringEntry is used to describe a substring. Here a substring is defined by its left and
  * right position in the original string and a "link" to the original string.
  */
struct t_SubStringEntry {
	unsigned       left;
	unsigned       right;
	const string * morph;
	t_SubStringEntry() : left(0), right(0), morph(0) {}	
	t_SubStringEntry(const unsigned & l, const unsigned & r, const string * m) : left(l), right(r), morph(m) {}
	t_SubStringEntry(const unsigned & l, const unsigned & r) : left(l), right(r), morph(0) {}
};
inline bool operator < (const t_SubStringEntry & a, const t_SubStringEntry & b)
{ return a.left < b.left; }
inline ostream & operator << (ostream & out, const t_SubStringEntry & e)
{
	if(e.morph)
		return out << *e.morph << " " << e.left << " " << e.right;
	return out << "(null)" << " " << e.left << " " << e.right;
}

#ifdef BONT
// struct for the morpheme lexicon
struct m_line {
  string orth;
  string trans;
  string m_class;
  m_line() {;}
  m_line(string o, string t, string c) : orth(o), trans(t), m_class(c) {;}
};
	
typedef vector< struct m_line> m_Vector; 			
#endif
typedef vector< t_SubStringEntry >   t_SubStringEntryVector;
typedef list< t_SubStringEntry >     t_SubStringEntryList;
typedef list< t_SubStringEntryList > t_SubStringEntryDList;
typedef deque< string >							 t_SDeque;
typedef map< string, string >	       t_SSMap;

/**
  *@author Karlheinz Stoeber
  * The lexicon contains all orthograpic morpheme strings given by the corresponding data
  * base table. To do a faster search in the "find" member function, the morphem lexicon is
  * represented by a set of subvectors. All morphemes of a subvectore have the properties that
  * their first character is equal.
  */
class BOSS_MorphemeLexicon_DE {
private:
	t_LexiconMap lexicon_map;
	BOSS::Config & cl;
	MySQL_Handler * data_base;
public:
	BOSS_MorphemeLexicon_DE(BOSS::Config & cl, MySQL_Handler * data_base);
	t_SubStringEntryVector find(const string & word) const;
};

/**
  *@author Karlheinz Stoeber
  * In this application the term morph is used in a slightly different way than the
  * defintion of a morpheme, because in some cases we are using a concatenation of morphemes
  * instead of single morphemes. This is very important for the selection mechanism
  * This class uses a lexicon to create a graph which contains all possible morpheme that can be
  * used to concatenate the word. Then a unique solution is searched in this graph. A
  * unique solution is a path which defines a set of morphemes. The concatenation of
  * all morpheme nodes must be identical to the word used as input.
  * The selection criteria are:
  *    Select the paths with the minimal number of morphemes
  *    Select the paths where the multiplication of the numbers of morpheme characters is minimal
  *    Select the path where the multiplication of the number of morpheme characters multiplied by
  *    the morpheme position is minimal
  *
  *@author Joerg Broeggelwirth
  * By concatenating morphs new syllable boundaries occur. These boundaries are basically
  * determined by the maximum onset principle. Moreover, the algorithms deal with the necessary
  * morphophonological processes, e.g. "Auslautverhärtung" (final devoicing).
  */

class BOSS_Morphemes_DE {
private:
	BOSS::Config & cl;
        MySQL_Handler * data_base;

	BOSS::FSA *pfsa;
	BOSS_MorphemeLexicon_DE lexicon;
	t_SSPairVector selectLong(t_SubStringEntryDList & slist) const;
	// Joerg
	bool is_vowel(const string & phone) const;
	void maximum_onset(BOSS::t_SVector & coda, t_SDeque & onset) const;
	inline string t_SVector_to_string(const BOSS::t_SVector & v) const {
		string result("");
		for (unsigned i=0; i<v.size(); i++) result+=v[i];
		return result;
	}
	BOSS::t_SVector try_parse(const string & phones) const;
	t_SSPairVector join_stems(const t_SSPairVector & trans) const;
	BOSS::t_SVector syllable_border(const t_SSPairVector & joined_trans) const;
	BOSS::t_SVector morpheme_border(const t_SSPairVector & joined_trans, const BOSS::t_SVector & syl_chain) const;
	BOSS::t_SVector apply_morphonology(const t_SSPairVector & trans, const BOSS::t_SVector & final_trans) const;
public:
        BOSS_Morphemes_DE(BOSS::Config & cl, MySQL_Handler * data_base);
	~BOSS_Morphemes_DE();
	string operator () (const string & word) const;
};

inline BOSS::t_SVector operator + (BOSS::t_SVector va, const BOSS::t_SVector & vb)
{
	for (unsigned i=0; i<vb.size(); i++) va.push_back(vb[i]);
	return va;
}	

inline BOSS::t_SVector operator + (BOSS::t_SVector va, const string & s)
{
	va.push_back(s);
	return va;
}

inline BOSS::t_SVector operator + (const t_SDeque & d, BOSS::t_SVector v)
{
	BOSS::t_SVector result;
	for (unsigned i=0; i<d.size(); i++) result.push_back(d[i]);
	return result+v;
}

inline BOSS::t_SVector operator + (BOSS::t_SVector v, const t_SDeque & d)
{
	for (unsigned i=0; i<d.size(); i++) v.push_back(d[i]);
	return v;
}

inline ostream& operator << (ostream & out, const BOSS::t_SVector & v)
{
	copy(v.begin(), v.end(), ostream_iterator<string>(out, ""));
	return out;
}

inline ostream& operator << (ostream & out, const t_SDeque & d)
{
	copy(d.begin(), d.end(), ostream_iterator<string>(out, " "));
	return out;
}

#ifdef BONT
/** Input/output operators for m_line */
inline istream & operator >> (istream & in, m_line & m) {
	return in >> m.orth >> m.trans >> m.m_class;
}

inline ostream & operator << (ostream & out, const m_line & m) {
	return out << m.orth << " " << m.trans << " " << m.m_class;
}

/** Input/output operators for p_Vector */
inline istream & operator >> (istream & in, m_Vector & v) {
	copy(istream_iterator< m_line >(in), istream_iterator< m_line >(), back_inserter(v));
	return in;
}

inline ostream & operator << (ostream & out, const m_Vector & v) {
	copy(v.begin(), v.end(), ostream_iterator< m_line >(out, "\n"));
	return out;
}
#endif

/** @} */ // end of boss_transcription group
#endif
