/* traing2p.cpp    $Revision: 1.4 $ $Date: 2008/04/05 07:37:42 $
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

#include "traing2p.h"

Traing2p::Traing2p(BOSS::Config & config, const string & traintype, const bool & createflag, const string & phonfile, const string & treefile, const string & lexfile, const string & featurefile, const LabelFormat & F): BOSS_g2p(config) {
  iniPointers();
  BOSS::t_SVector p=read_phoneset(phonfile);
  if (traintype == "phon") {
    g2ppfsa = new BOSS::FSA(p);
    for (unsigned i=0;i<p.size();i++)
      pv.push_back(remove_quotes(p[i]));
    pv.push_back(NULL_PHONEME);
    if (createflag) create_tree(featurefile,treefile);
    else trainphon(lexfile, treefile, featurefile, F);
  }
  else if (traintype == "syll"){
    syllpfsa = new BOSS::FSA(p);
    if (createflag) create_tree(featurefile,treefile);
    else trainsyll(lexfile, treefile, featurefile, F);
  }
  else if (traintype == "stress"){
    stresspfsa = new BOSS::FSA(p);
    if (createflag) create_tree(featurefile,treefile);
    else trainstress(lexfile, treefile, featurefile, F);
  }
}

void Traing2p::trainphon(const string & lexfile, const string & treefile, const string & featurefile, const LabelFormat & F) {
  // cerr << "Called Traing2p::trainphon" << endl;
  ifstream lexicon(lexfile.c_str());
  if (!lexicon) {
    cerr << "Cannot open " << lexfile << "!" << endl;
    exit(-1);
  }
  t_MappingTableEntry mte;
  t_MappingTableMap   mtev;
  cerr << "Reading lexicon..." << endl;
  while(lexicon >> mte) {
    if (mte.target == "")
      cerr << "No transcription for " << mte.key << endl;
    mtev[BOSS::my_tolower(mte.key)] = clean_blftrans(mte.target);
  }
  lexicon.close();
  cerr << "Extracting graphemes..." << endl;
  BOSS::t_SVector gv=extract_graphemes(mtev);
  cerr << "Initializing BOSS_g2pMatrix..." << endl;
  BOSS_g2pMatrix gpm(gv,pv);
  unsigned i(0),j(0);
  for (t_MappingTableMap::iterator I = mtev.begin();I != mtev.end();I++) {
    cerr << "Aligning and assigning " << ++i << "/" << mtev.size() << ": ";
    assign_scores(align_pair((*I).first,(*I).second,gpm),gpm);
    cerr << endl;
  }
  cerr << "Computing probabilities..." << endl;
  gpm.compute_prob();
  vector <BOSS::t_SVector> res;
  t_Alignment al;
  ofstream features(featurefile.c_str());
  i=0;
  for (t_MappingTableMap::iterator I = mtev.begin();I != mtev.end();I++) {
    cerr << "Finding best alignment for " << ++i << "/" << mtev.size() << ": ";
    al = find_bestAlignment(align_pair((*I).first,(*I).second,gpm),gpm);
    i++;
    if (al.accepted) {
      cerr << " OK! (" << ++j << "/" << mtev.size()-i << ")";
      res = generateG2PFeatures(al.graph,al.best);
      for (unsigned i=0;i<res.size();i++)
      {
	for (unsigned j = 0; j < res[i].size() - 1; j++) features << res[i][j] << " "; //features
	features << res[i][res[i].size() - 1]; //class
	features << endl;
      }
    }
    cerr << endl;
  }
  features.close();
  create_tree(featurefile,treefile);
}

void Traing2p::trainsyll(const string & lexfile, const string & treefile, const string & featurefile, const LabelFormat & F) {
  // cerr << "Called Traing2p::trainsyll" << endl;
  ifstream lexicon(lexfile.c_str());
  if (!lexicon) {
    cerr << "Can't open " << lexfile << "!" << endl;
    exit(-1);
  }
  t_MappingTableEntry mte;
  BOSS::t_SVector trans;
  cerr << "Reading lexicon..." << endl;
  while(lexicon >> mte) {
    if (mte.target == "")
      cerr << "No transcription for " << mte.key << endl;
    trans.push_back(clean_blftrans(mte.target, 1));
  }
  lexicon.close();
  ofstream features(featurefile.c_str());
  BOSS::t_SVector t;
  for (unsigned i=0;i<trans.size();i++) {
    cerr << "Creating feature vectors from transcription " << i+1 << " out of " << trans.size() << ". " << "trans[" << i << "]: " << trans[i] << endl;
    t = syllpfsa->parse(trans[i]);
    BOSS::t_SVector res = generateSyllFeatures(t);
    for (unsigned j=0;j<res.size();j++) features << res[j] << endl;
  }
  features.close();
  create_tree(featurefile,treefile);
}

void Traing2p::trainstress(const string & lexfile, const string & treefile, const string & featurefile,const LabelFormat & F) {
  // cerr << "Called Traing2p::trainstress" << endl;
  ifstream lexicon(lexfile.c_str());
  if (!lexicon) {
    cerr << "Can't open " << lexfile << "!" << endl;
    exit(-1);
  }
  t_MappingTableEntry mte;
  BOSS::t_SVector trans;
  cerr << "Reading lexicon..." << endl;
  while(lexicon >> mte) {
    if (mte.target == "")
      cerr << "No transcription for " << mte.key << endl;
    trans.push_back(mte.target);
  }
  lexicon.close();
  ofstream features(featurefile.c_str());
  BOSS::t_SVector t;
  for (unsigned i=0;i<trans.size();i++) {
    cerr << "Creating feature vectors from transcription " << i+1 << " out of " << trans.size() << ". " << "trans[" << i << "]: " << trans[i] << endl;
    t = stresspfsa->parse(trans[i]);
    BOSS::t_SVector res = generateStressFeatures(t);
    for (unsigned j=0;j<res.size();j++) features << res[j] << endl;
  }
  features.close();
  create_tree(featurefile,treefile);
}

void Traing2p::create_tree(const string & featurefile, const string & treefile) {
  cerr << "Building the tree..." << endl;
  phimbl.Learn(featurefile);
  cerr << "Writing the tree..." << endl;
  phimbl.Write(treefile);
}

string Traing2p::clean_blftrans(const string & s) {
  //    cerr << "Called Traing2p::clean_blftrans" << endl;
  unsigned j=0;
  string res(s);
  for(unsigned i=0; i<s.size(); i++) {
    if(s[i] == '"' || s[i] == '%' || s[i] == '.' || s[i] == ',' || s[i] == ';' || s[i] == '#')
      continue;
    res[j++] = s[i];
  }
  res.resize(j);
  return res;
}

string Traing2p::clean_blftrans(const string & s, bool syll_flag) {
  //    cerr << "Called Traing2p::clean_blftrans" << endl;
  unsigned j=0;
  string res(s);
  for(unsigned i=0; i<s.size(); i++) {
    if(s[i] == '"' || s[i] == '%' || s[i] == ',')
      continue;
    if (s[i] == ';' || s[i] == '#') {
      res[j++] = '.';
      continue;
    }
    res[j++] = s[i];
  }
  res.resize(j);
  return res;
}

string Traing2p::remove_quotes(const string & s) {
  //    cerr << "Called Traing2p::remove_quotes" << endl;
  unsigned j=0;
  string res(s);
  for(unsigned i=0; i<s.size(); i++) {
    if(s[i] == '\\' && i+1<s.size() && s[i+1] != '\\')
      continue;
    res[j++] = s[i];
  }
  res.resize(j);
  return res;
}

BOSS::t_SVector Traing2p::extract_graphemes(t_MappingTableMap & mt) {
  //  cerr << "Called Traing2p::extract_graphemes" << endl;
  BOSS::t_SVector res;
  string t(" ");
  t_GPTableMap gmap;
  for (t_MappingTableMap::iterator Imt = mt.begin();Imt != mt.end();Imt++) {
    const string & tmp=(*Imt).first;
    for (unsigned i=0; i<tmp.size(); i++) {
      t[0] = tmp[i];
      if (gmap.find(t) == gmap.end()) {
	gmap[t] = 1;
      }
      else {
	gmap[t]+=1;
      }
    }
  }
  for (t_GPTableMap::iterator Igmap = gmap.begin(); Igmap != gmap.end(); Igmap++)
    res.push_back((*Igmap).first);
  return res;
}

t_Alignment Traing2p::align_pair(const string & graph, const string & phon, BOSS_g2pMatrix & gpm) {
  //       cerr << "Called Traing2p::align_pair for word " << graph << " with transcription " << phon << endl;
  cerr << graph << " " << phon;
  t_Alignment al;
  al.word=graph;
  vector<BOSS::t_SVector> res;
  BOSS::t_SVector gv;
  vector<BOSS::t_SVector> perm;
  for (unsigned i=0; i<graph.size(); i++) {
    ostringstream tmp;
    tmp << graph[i];
    gv.push_back(tmp.str());
  }
  al.graph = gv;
  // cerr << phon << endl;
  BOSS::t_SVector pv = g2ppfsa->parse(phon);
  int d = gv.size()-pv.size();
  vector<bool> piv;
  if (d>0) {
    if (binomial((double)gv.size(),(double)d) > MAX_NUMBER_OF_ALIGNMENTS) {
      al.accepted = false;
      return al;
    }
    for (unsigned i=0;i<gv.size();i++) {
      if ((int)i<d) {
	piv.push_back(0);
      }
      else {
	piv.push_back(1);
      }
    }
    // unsigned n = 0;
    do {
      BOSS::t_SVector rsv;
      BOSS::t_SVector cpv = pv;
      //  cerr << ++n << ":";
      unsigned j = 0;
      for (unsigned i=0;i<piv.size();i++) {
	if (piv[i]) {
	  rsv.push_back(cpv[j++]);
	  //  cerr << " " << cpv[j-1];
	}
	else {
	  rsv.push_back(NULL_PHONEME);
	  //  cerr << " _";
	}
      }
      //  cerr << endl;
      res.push_back(rsv);
    } while (next_permutation(piv.begin(),piv.end()));
  }
  else res.push_back(pv);
  al.accepted=true;
  al.phon = res;
  //    al.size();
  return al;
}

void Traing2p::assign_scores(const t_Alignment & al, BOSS_g2pMatrix & gpm) {
  // cerr << "Called Traing2p::assign_scores for \"" << al.word << "\"" << endl;
  BOSS::t_SVector g = al.graph;
  if (!al.accepted) {
    return;
  }
  vector< BOSS::t_SVector > vp = al.phon;
  for (unsigned a=0;a<vp.size();a++) {
    BOSS::t_SVector p = al.phon[a];
    for (unsigned i=0; i<g.size();i++) {
      gpm.add_value(g[i],p[i],8);
      if (i>0) {
	gpm.add_value(g[i],p[i-1],4);
	if (i>1) {
	  gpm.add_value(g[i],p[i-2],2);
	  if (i>2) {
	    gpm.add_value(g[i],p[i-3],1);
	  }
	}
      }
    }
  }
}

t_Alignment Traing2p::find_bestAlignment(t_Alignment al, BOSS_g2pMatrix & gpm) {
  // cerr << "Called Traing2p::find_bestAlignment for word " << al.word << endl;
  if (!al.accepted) {
    return al;
  }
  BOSS::t_SVector best;
  BOSS::t_SVector graph = al.graph;
  vector<BOSS::t_SVector> phon = al.phon;
  double prob;
  double best_total=-1.0;
  //  cerr << "phon.size() / graph.size() " << phon.size() << " / " << graph.size() << endl;
  for (unsigned i=0;i<phon.size();i++) {
    prob=0;
    //        cerr << i << ":";
    for (unsigned j=0;j<graph.size();j++) {
      prob+=(gpm.get_value(graph[j],(phon[i])[j]).second);
      //            cerr << " " << (phon[i])[j];
    }
    //        cerr << " (prob: " << prob << ")" << endl;
    if (prob>best_total) {
      best=phon[i];
      best_total=prob;
    }
  }
  //  cerr << "best:";
  for (unsigned i=0;i<best.size();i++)
    //  cerr << " " << best[i];
    //  cerr << endl;
    al.best = best;
  return al;
}

void Traing2p::print_matrix(BOSS_g2pMatrix & m) {
  //  cerr << "Called Traing2p::print_matrix" << endl;
  BOSS::t_SVector v = m.get_matrix();
  for (unsigned i=0;i<v.size();i++) {
    cout << v[i] << endl;
  }
}

double Traing2p::binomial(const double & n, const double k) {
  if (n > 1 && k > 1) {
    return ((n/k)*binomial(n-1,k-1));
  }
  else {
    return (n/k);
  }
}

BOSS::t_SVector Traing2p::generateSyllFeatures(const BOSS::t_SVector & t) {
  // cerr << "Called Traing2p::generateSyllFeatures" << endl;
  BOSS::t_SVector res, sf;
  for (unsigned j=0;j<t.size();j++) {
    ostringstream fs;
    cerr << "Creating vector for t[" << j << "]: " << t[j] << endl;
    if (t[j] == SYLLABLE_BOUNDARY_TAG) continue;
    sf = string2SyllFeature(t,j);
    for (unsigned i = 0; i < sf.size(); i++) fs << sf[i] << " ";
    res.push_back(fs.str());
  }
  return res;
}

BOSS::t_SVector Traing2p::generateStressFeatures(const BOSS::t_SVector & t) {
  // cerr << "Called Traing2p::generateStressFeatures" << endl;
  BOSS::t_SVector res, sf;
  for (int j=int(t.size()-1);j>=0;j--) {
		ostringstream fs;
    cerr << "Creating vector for t[" << j << "]: " << t[j] << endl;
    if (t[j] == PRIMARY_LEXICAL_STRESS_TAG || t[j] == SECONDARY_LEXICAL_STRESS_TAG) continue;
    sf = string2StressFeature(t,j);
    for (unsigned i = 0; i < sf.size(); i++)  fs << sf[i] << " ";
    res.push_back(fs.str());
  }
  return res;
}

