/* phimbl.h    $Revision: 1.3 $ $Date: 2007/04/16 09:43:33 $
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
   @author Philip Gross
*/

#ifndef PHIMBL_H
#define PHIMBL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "boss_utility/boss_types.h"

using namespace std;

namespace BOSS
{
class Phimbl
{
protected:
  struct t_Node
  {
    t_Node* parent;
    int Class;
    //int max;
    map <int,int> count;
    struct t_Child
    {
      int Feature;
      t_Node* node;
    };
    vector <t_Child*> child;
    t_Node(){parent = NULL;}
  };

 public:
  struct t_Data
  {
    t_Node* currentNode;
    t_Node* root;
    vector <int> permutation;
    vector <string> classes;
    vector <string> features;
    t_Data(){currentNode = root = NULL;}
  };

 protected:
  struct t_classes
  {
    string class_string;
    int count;
    vector <string> features;
  };

  struct t_features
  {
    string feature_string;
    int count;
  };

  vector <t_classes> classes;
  vector <vector <t_features> > features;
  vector <t_features> allfeatures;
  int feature_count;
  int line_count;
  string seperator;
  t_Node* tree;
  t_Data Data;
  
  int find_str(string & str, vector <t_classes> & data);
  int find_str(string & str, vector <t_features> & data);

  void ReduceTree(t_Node * node);
  void DeleteNode(t_Node * node);
  void SaveTree(t_Node * node, fstream & out);
  bool Load(string filename);
  void Calculate_Permutation();
  void Bulid_Tree(string filename);
  void ClassCount(t_Node * node);
  bool Save(string outfile);
  void Print_Permutation();
  void AddChild(t_Node* parent, string & str, int pos, t_Data & Data);

public:
  Phimbl();
  ~Phimbl();

  vector <int> permutation;
  bool Learn(const string & filename);
  bool Write(const string & filename);
  bool Classify(const BOSS::t_SVector & str, string & res, const t_Data & Data) const;
  bool operator () (string & filename);
  bool LoadTree(const char* filename, t_Data & Data);
  void DeleteData(t_Data & Data);
};
}

#endif
