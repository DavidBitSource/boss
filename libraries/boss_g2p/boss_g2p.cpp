/* boss_g2p.cpp    $Revision: 1.6 $ $Date: 2010/11/24 11:39:30 $
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
 * @file boss_g2p.cpp
   @author Stefan Breuer
   @author Philip Gross
   @author Filip Müllers
 */

#include "boss_g2p.h"

using namespace std;

BOSS_g2p::BOSS_g2p(BOSS::Config & __cl) : cl(__cl) {
    iniPointers();
}

BOSS_g2p::BOSS_g2p(BOSS::Config & __cl, const LabelFormat & F) : cl(__cl) {
    iniPointers();
    //temporary measure, as long as no SQL tables are available:
    const string g2pphonfile = cl("-g2pphon").optionarg;
    const string syllphonfile = cl("-syllphon").optionarg;
    const string stressphonfile = cl("-stressphon").optionarg;
    const string g2ptreefile = cl("-g2ptree").optionarg;
    const string sylltreefile = cl("-sylltree").optionarg;
    const string stresstreefile = cl("-stresstree").optionarg;
    g2ppfsa = new BOSS::FSA(read_phoneset(g2pphonfile));
    syllpfsa = new BOSS::FSA(read_phoneset(syllphonfile));
    stresspfsa = new BOSS::FSA(read_phoneset(stressphonfile));
    phimbl.LoadTree(g2ptreefile.c_str(), g2pData);
    phimbl.LoadTree(sylltreefile.c_str(), syllData);
    phimbl.LoadTree(stresstreefile.c_str(), stressData);
}

BOSS_g2p::BOSS_g2p(const BOSS_g2p & c) : cl(c.cl) {
    g2ppfsa = new BOSS::FSA(*(c.g2ppfsa));
    syllpfsa = new BOSS::FSA(*(c.syllpfsa));
    stresspfsa = new BOSS::FSA(*(c.stresspfsa));
    pv = c.pv;
}

BOSS_g2p & BOSS_g2p::operator =(const BOSS_g2p & c) {
    delPointers();
    g2ppfsa = new BOSS::FSA(*(c.g2ppfsa));
    syllpfsa = new BOSS::FSA(*(c.syllpfsa));
    stresspfsa = new BOSS::FSA(*(c.stresspfsa));
    pv = c.pv;
    return *this;
}

BOSS_g2p::~BOSS_g2p() {
    delPointers();
}

BOSS::t_SVector BOSS_g2p::read_phoneset(const string & phonfile) {
    ifstream phones(phonfile.c_str());
    if (!phones) {
        cerr << "Couldn't read " << phonfile << "!" << endl;
        exit(-1);
    }
    BOSS::t_SVector res;
    string tmp;
    t_GPTableMap pmap;
    while (phones >> tmp) {
        if (pmap.find(tmp) == pmap.end()) {
            pmap.insert(pair<string, unsigned>(tmp, 1));
        } else {
            pmap[tmp] += 1;
        }
    }
    phones.close();
    for (t_GPTableMap::iterator Ipmap = pmap.begin(); Ipmap != pmap.end(); Ipmap++)
        res.push_back((*Ipmap).first);
    return res;
}

vector< BOSS::t_SVector > BOSS_g2p::generateG2PFeatures(BOSS::t_SVector g, BOSS::t_SVector p) const {
    // cerr << "Called BOSS_g2p::generateG2PFeatures" << endl;
    vector < BOSS::t_SVector > res;
    BOSS::t_SVector s;
    while (p.size() < g.size())
        p.push_back(NULL_PHONEME);
    for (int i = 0; i < (int) g.size(); i++) {
        s.clear();
        for (int j = TRANS_TRAIN_WINDOW; j >= 0; j--) {
            if (i - j >= 0) {
                s.push_back(g[i - j]);
            } else s.push_back("#");
        }
        for (int j = 1; j <= TRANS_TRAIN_WINDOW; j++) {
            if (i + j < (int) g.size()) {
                s.push_back(g[i + j]);
            } else s.push_back("#");
        }
        s.push_back(p[i]);
        res.push_back(s);
    }
    return res;
}

BOSS::t_SVector BOSS_g2p::string2SyllFeature(const BOSS::t_SVector & t, const unsigned & j) const {
    BOSS::t_SVector pre, post, res;
    //  cerr << "Features before unit:" << endl;
    for (unsigned k = 1; k <= SYLL_TRAIN_WINDOW; k++) {
        if (int(j) - int(k) >= 0) {
            //      cerr << j << "-" << k << "=" << int(j)-int(k) << ", t[" << j-k << "]: " << t[j-k] << endl;
            pre.push_back(t[j - k]);
        } else {
            pre.push_back(WORD_BOUNDARY_TAG);
            break;
        }
    }
    unsigned wlim = SYLL_TRAIN_WINDOW;
    //   cerr << "Features after unit:" << endl;
    for (unsigned k = 1; k <= wlim; k++) {
        //     cerr << j+k << endl;
        if (j + k < t.size()) {
            //      cerr << j << "+" << k << "=" << int(j)+int(k) << ", t[" << j+k << "]: " << t[j+k] << endl;
            if (t[j + k] == SYLLABLE_BOUNDARY_TAG) {
                wlim++;
                continue;
            }
            post.push_back(t[j + k]);
        } else {
            post.push_back(WORD_BOUNDARY_TAG);
            break;
        }
    }
    //   cerr << "Adding NULL_PHONEMES to 'pre'-features" << endl;
    while (pre.size() < SYLL_TRAIN_WINDOW) pre.push_back(NULL_PHONEME);
    //   cerr << "Adding NULL_PHONEMES to 'post'-features" << endl;
    while (post.size() < SYLL_TRAIN_WINDOW) post.push_back(NULL_PHONEME);
    //   cerr << "Printing 'pre'-features" << endl;
    for (unsigned k = 0; k < SYLL_TRAIN_WINDOW; k++) res.push_back(pre[(SYLL_TRAIN_WINDOW - 1) - k]);
    //   cerr << "Printing observed unit" << endl;
    res.push_back(t[j]);
    //   cerr << "Printing 'post'-features" << endl;
    for (unsigned k = 0; k < SYLL_TRAIN_WINDOW; k++) res.push_back(post[k]);
    if (j + 1 < t.size()) {
        if (t[j + 1] == SYLLABLE_BOUNDARY_TAG) {
            res.push_back(TIMBLCLASS_SYLLABLE_BOUNDARY_PRESENT);
        } else res.push_back(TIMBLCLASS_SYLLABLE_BOUNDARY_ABSENT);
    } else res.push_back(TIMBLCLASS_SYLLABLE_BOUNDARY_PRESENT);
    //    for (unsigned i=0;i<res.size();i++)
    //  cerr << res[i] << " ";
    return res;
}

BOSS::t_SVector BOSS_g2p::string2StressFeature(const BOSS::t_SVector & t, const unsigned & j) const {
    BOSS::t_SVector pre, post, res;
    // cerr << "Features before unit:" << endl;
    unsigned wlim = STRESS_TRAIN_WINDOW;
    for (unsigned k = 1; k <= wlim; k++) {
        if (int(j) - int(k) >= 0) {
            // cerr << j << "-" << k << "=" << int(j)-int(k) << ", t[" << j-k << "]: " << t[j-k] << endl;
            if (t[j - k] == PRIMARY_LEXICAL_STRESS_TAG || t[j - k] == SECONDARY_LEXICAL_STRESS_TAG) {
                wlim++;
                continue;
            }
            pre.push_back(t[j - k]);
        } else {
            pre.push_back(WORD_BOUNDARY_TAG);
            break;
        }
    }
    // cerr << "Features after unit:" << endl;
    for (unsigned k = 1; k <= STRESS_TRAIN_WINDOW; k++) {
        // cerr << j+k << endl;
        if (j + k < t.size()) {
            // cerr << j << "+" << k << "=" << int(j)+int(k) << ", t[" << j+k << "]: " << t[j+k] << endl;
            post.push_back(t[j + k]);
        } else {
            post.push_back(WORD_BOUNDARY_TAG);
            break;
        }
    }
    // cerr << "Adding NULL_PHONEMES to 'pre'-features" << endl;
    while (pre.size() < STRESS_TRAIN_WINDOW) pre.push_back(NULL_PHONEME);
    // cerr << "Adding NULL_PHONEMES to 'post'-features" << endl;
    while (post.size() < STRESS_TRAIN_WINDOW) post.push_back(NULL_PHONEME);
    // cerr << "Printing 'pre'-features" << endl;
    for (unsigned k = 0; k < STRESS_TRAIN_WINDOW; k++) res.push_back(pre[(STRESS_TRAIN_WINDOW - 1) - k]);
    // cerr << "Printing observed unit" << endl;
    res.push_back(t[j]);
    // cerr << "Printing 'post'-features" << endl;
    for (unsigned k = 0; k < STRESS_TRAIN_WINDOW; k++) res.push_back(post[k]);
    if (j > 0) {
        if (t[j - 1] == PRIMARY_LEXICAL_STRESS_TAG) res.push_back(TIMBLCLASS_PRIMARY_LEXICAL_STRESS_PRESENT);
        else if (t[j - 1] == SECONDARY_LEXICAL_STRESS_TAG) res.push_back(TIMBLCLASS_SECONDARY_LEXICAL_STRESS_PRESENT);
        else res.push_back(TIMBLCLASS_LEXICAL_STRESS_ABSENT);
    } else res.push_back(TIMBLCLASS_LEXICAL_STRESS_ABSENT);
    //  for (unsigned i=0;i<res.size();i++) cerr << res[i] << " ";
    return res;
}

string BOSS_g2p::transcribe(const string & s) const {
    //    cerr << "Called BOSS_g2p::transcribe for \"" << s << "\"" << endl;
    return postProcTrans(assign_stress(syllabify(g2p(s))));
}

string BOSS_g2p::g2p(const string & s) const {
    //cerr << "Transcribing " << s << endl;
    BOSS::t_SVector g, p;
    vector< BOSS::t_SVector > f;
    ostringstream res;
    string t = " ";
    string w = BOSS::my_tolower(s);

    /*for (unsigned i=0;i<w.size();i++) {
      t[0]=w[i];
      g.push_back(t);
    }*/
    for (unsigned i = 0; i < w.length(); i++) {
        if (w.at(i) != -61) {
            t = w[i];
        } else {
            i++;
            if (w.at(i) == -74) {
                t = "ö";
            } else if (w.at(i) == -92) {
                t = "ä";
            } else if (w.at(i) == -68) {
                t = "ü";
            } else if (w.at(i) == -97) {
                t = "ß";
            } else if (w.at(i) == -124) {
                t = "Ä";
            } else if (w.at(i) == -106) {
                t = "Ö";
            } else if (w.at(i) == -100) {
                t = "Ü";
            }
        }
        g.push_back(t);
    }

    f = generateG2PFeatures(g, p);
    for (unsigned i = 0; i < f.size(); i++) {
        string r;
//            for (unsigned j=0;j<f[i].size();j++) cerr << f[i][j] << " ";
        phimbl.Classify(f[i], r, g2pData);
//            cerr << " -> \"" << r << "\"" << endl;
        if (string(r) != NULL_PHONEME) {
            res << r;
        }
    }
    return postProcPhon(res.str());
}

string BOSS_g2p::syllabify(const string & s) const {
    //  cerr << "Syllabifying " << s << endl;
    BOSS::t_SVector t = syllpfsa->parse(s);
    int i = 0;
    string r;
    for (BOSS::t_SVector::iterator I = t.begin(); I != t.end(); I++) {
        if (i<int(t.size()) - 1) {
            phimbl.Classify(string2SyllFeature(t, i), r, syllData);
            //      cerr << endl << " --> " << r << endl;
            if (r == TIMBLCLASS_SYLLABLE_BOUNDARY_PRESENT) {
                I = t.insert(++I, SYLLABLE_BOUNDARY_TAG);
                i++;
            }
        } else break;
        i++;
    }
    ostringstream trans;
    for (unsigned j = 0; j < t.size(); j++) trans << t[j];
    return postProcSyll(trans.str());
}

// ****************************************************************************
// ****************************************************************************

string BOSS_g2p::assign_stress(const string & s) const {
    //  cerr << "Assigning stress to " << s << endl;

    BOSS::t_SVector t = stresspfsa->parse(s);
    int i = t.size() - 1;
    string r;
    bool finished = false;
    BOSS::t_SVector::iterator I;

    for (I = t.end(); I != t.begin(); I--) {
        if (i >= 0) {
            phimbl.Classify(string2StressFeature(t, i), r, stressData);
            //cerr << endl << " --> " << r << endl;
            if (r == TIMBLCLASS_PRIMARY_LEXICAL_STRESS_PRESENT) {
                //cerr << "Primary stress detected" << endl;
                I = t.insert(--I, PRIMARY_LEXICAL_STRESS_TAG);
                i--;
            } else
                if (r == TIMBLCLASS_SECONDARY_LEXICAL_STRESS_PRESENT) {
                //cerr << "Secondary stress detected" << endl;
                I = t.insert(--I, SECONDARY_LEXICAL_STRESS_TAG);
                i--;
            }

            if (I == t.begin()) {
                finished = true;
            } // Important!! Otherwise runtime error !
        } else break;

        if (finished) break; // Important!! Otherwise runtime error !
        i--;
    }

    ostringstream trans;
    for (unsigned j = 0; j < t.size(); j++) {
        trans << t[j];
    }

    return postProcStress(trans.str());
}

// ****************************************************************************
// ****************************************************************************

void BOSS_g2p::iniPointers() {
    g2ppfsa = 0;
    syllpfsa = 0;
    stresspfsa = 0;
}

void BOSS_g2p::delPointers() {
    if (g2ppfsa) delete g2ppfsa;
    if (syllpfsa) delete syllpfsa;
    if (stresspfsa) delete stresspfsa;
    phimbl.DeleteData(g2pData);
    phimbl.DeleteData(syllData);
    phimbl.DeleteData(stressData);
}
