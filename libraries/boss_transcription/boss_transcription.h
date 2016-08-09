/* boss_transcription.h    $Revision: 1.3 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_transcription.h
   @author Karlheinz Stoeber
   @author Joerg Broeggelwirth
   @author Stefan Breuer
*/

#ifndef BOSS_TRANSCRIPTION_H
#define BOSS_TRANSCRIPTION_H

#include <string>
#include "xercesc/dom/DOM.hpp"
#include "boss_utility/boss_fsa.h"
#include "boss_utility/boss_module.h"
#include "boss_context/boss_context.h"
#include "boss_utility/boss_config.h"
#ifndef BONT
  #include "boss_mysql/boss_mysql_handler.h"
  #include "boss_mysql/boss_mysql_result.h"
#endif

using namespace std;
//using namespace xercesc;


/**
 * @defgroup boss_transcription BOSS Transcription Module
 * @{
 */

#ifdef BONT
struct p_line {
  string phon;
  string p_class;
  string voicing;
  p_line() {;}
  p_line(string p, string c, string v) : phon(p), p_class(c), voicing(v) {;}
};

// data type for the phone table
typedef vector<struct p_line> p_Vector;
typedef map< string, string > t_SSMap;

/** Input/output operators for p_line */
inline istream & operator >> (istream & in, p_line & p) {
	return in >> p.phon >> p.p_class >> p.voicing;
}

inline ostream & operator << (ostream & out, const p_line & p) {
	return out << p.phon << " " << p.p_class << " " << p.voicing;
}

/** Input/output operators for p_Vector */
inline istream & operator >> (istream & in, p_Vector & v) {
	copy(istream_iterator< p_line >(in), istream_iterator< p_line >(), back_inserter(v));
	return in;
}

inline ostream & operator << (ostream & out, const p_Vector & v) {
	copy(v.begin(), v.end(), ostream_iterator< p_line >(out, "\n"));
	return out;
}
#endif



/**
 * @brief Base class for transcription modules
 */
class BOSS_Transcription : public BOSS::Module {

protected: 
	unsigned type_flag;
#ifdef BONT
  t_SSMap e_table;
#endif
	BOSS::FSA  * pfsa;
	BOSS_context * cclass;
#ifdef BONT
	virtual string queryExceptionLexicon(const string & item) const;
#else
	virtual MySQL_Result queryExceptionLexicon(const string & table_name, const string & item) const;
#endif
	virtual string delMorphBound(string & s) const;
#ifndef BONT
	virtual void stringToDOM(const string & trans, xercesc::DOMNode * word) const;
	virtual void addPhonemeContext(xercesc::DOMNode * sentence) const;
	virtual void addPhrases(xercesc::DOMNode * sentence) const;
	virtual void addPhonemeNumber(xercesc::DOMNode * sentence) const;
	virtual void addHalfphones(xercesc::DOMNode * sentence) const;
#endif
	
public:
	
	BOSS_Transcription(BOSS::Config & c, MySQL_Handler * db);
	virtual ~BOSS_Transcription();
	virtual void setFlag(const unsigned &);
	virtual string getModuleType() {return "transcription"; }
//	virtual void operator () (xercesc::DOMNode * sentence) = 0;
};

/** @} */ // end of boss_transcription group
#endif
