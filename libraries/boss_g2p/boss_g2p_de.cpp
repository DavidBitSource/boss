/* boss_g2p_de.cpp    $Revision: 1.3 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_g2p_de.cpp
   @author Stefan Breuer
*/

#include "boss_g2p_de.h"

string BOSS_g2p_DE::postProcStress(const string & s) const {
	bool pflag = false;
	bool sflag = false;
	unsigned pstress = 0;
	unsigned sstress = 0;
	unsigned syllable = 1;
	ostringstream res;
	for (unsigned i=0;i<s.size();i++) {
	    if (s[i] == '.' || s[i] == ';') {
	    	syllable++;
	    	res << s[i];
	    	continue;
	    }	    	
		if (s[i] == '"') {
			if (pflag) {
				res << '%';
				continue;
			}
			pflag = true;
			if (pstress == syllable) continue;
			pstress = syllable;
		}
		else {
			if (s[i] == '%') {
				sflag = true;
				if (sstress == syllable) continue;
				sstress = syllable;
			}
		}
		res << s[i];
	}
	if (!pflag) {
		 BOSS::t_SVector v = stresspfsa->parse(res.str());
		 ostringstream res2;
		 if (!sflag) {
		 	bool breakflag = false;
			for (unsigned i=0;i<v.size();i++) {
				for (unsigned j=0;j<v[i].size() && !breakflag;j++) {
					switch (v[i][j]) {
		    			case ':':
						case '@':
						case '6':
						case 'i':
						case 'I':
						case 'y':
						case 'Y':
						case 'e':
						case 'E':
						case '2':
						case '9':
						case 'a':
						case 'o':
						case 'O':
						case 'u':
						case 'U':
							v[i] = '"' + v[i];
							breakflag = true;
							break;
					}
				}
			}
			for (unsigned i=0;i<v.size();i++) {
				res2 << v[i];
			}
		 }
		 else {
		 	for (unsigned i=0;i<v.size();i++) {
				for (unsigned j=0;j<v[i].size();j++) {
					if (v[i][j] == '%' && !pflag){
						res2 << '"';
						pflag = true;
					}
					else res2 << v[i][j];
				}
			}
		}
		return res2.str();
	}
	return res.str();
}

string BOSS_g2p_DE::postProcSyll(const string & s) const {	
	int sylli=s.size();
	bool nucflag = false;	
	vector<int> iv;
	for (int i=(int)s.size();i>=0;i--) {
		switch (s[i]) {
			case '.':
				if (!nucflag) {
					iv.push_back(i);
					continue;
				}
				nucflag = false;
				sylli = i;
				break;
			case ':':
			case '@':
			case '6':
			case 'i':
			case 'I':
			case 'y':
			case 'Y':
			case 'e':
			case 'E':
			case '2':
			case '9':
			case 'a':
			case 'o':
			case 'O':
			case 'u':
			case 'U':
				nucflag=true;
				break;
		}					
	}
	if (!nucflag && (unsigned)sylli<s.size()) iv.push_back(sylli);
	if (iv.size()) {
		ostringstream res;
		int j=(int)iv.size()-1;
		for (unsigned i=0;i<s.size();i++) {
			if (j>=0) {
				if (iv[j]==(int)i) {
					j--;
					continue;
				}
			}
			res << s[i];
		}
		return reduceGeminates(res.str());
	}
	return reduceGeminates(s);
}

string BOSS_g2p_DE::reduceGeminates(const string & s) const {
  ostringstream res;
  for (unsigned i=0;i<s.size();i++) {
    if (i<s.size()-1 && s[i]==s[i+1]) continue;
    res << s[i];
  }
  return res.str();
}

