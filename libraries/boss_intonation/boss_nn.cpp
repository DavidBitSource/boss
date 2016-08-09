/* boss_intonation_de.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
 * @file boss_nn.cpp
   @author Philip Gross
*/

#include "boss_nn.h"
#include <sstream>
#include <math.h>

/**
 * @addtogroup boss_intonation
 * @{
 */

/// @brief BOSS_NN constructor.
BOSS_NN::BOSS_NN()
{
  NumberOfLayers=0;
}

/// @brief BOSS_NN destructor.
BOSS_NN::~BOSS_NN()
{

// TODO: remove whole destructor if necessary, mho
  for(int i=0;i < NumberOfLayers-1;i++)
    {
      for(int j=0;j < layer[i].NumberOfNeurons;j++)
	{
	  //free(layer[i].neuron[j].weight);
	}
      //free(layer[i].neuron);
    }
//  free(layer[NumberOfLayers-1].neuron);
//  free(layer);
}

bool BOSS_NN::LoadNN(const string & filename)
{
  //cout << "Entering LoadNN(" << filename << ")" << endl;
  int t;
  string str;
  char* strEnd;
  srand (time(NULL));
  ifstream in(filename.c_str());
  if (in)
    {
      getline(in,str);
      NumberOfLayers=atoi(str.c_str());
      layer= vector<t_layer>(NumberOfLayers);
      for(int i=0;i < NumberOfLayers;i++)
	{
	  getline(in,str);
	  t = atoi(str.c_str());
	  layer[i].NumberOfNeurons = t;   //# of neurons in layer
	  layer[i].neuron = vector<t_neuron>(t);
	  if (i > 0) layer[i-1].NumberOfWeights=t;
	}
      layer[NumberOfLayers-1].NumberOfWeights = 0;
      for(int i=0;i < NumberOfLayers-1;i++)
	{
	  for(int j=0;j < layer[i].NumberOfNeurons;j++)
	    {
	      layer[i].neuron[j].weight = vector<float>(layer[i].NumberOfWeights);
	      layer[i].neuron[j].deltaw = 0; //(float*) calloc(layer[i].NumberOfWeights,sizeof(float));
	      layer[i].neuron[j].out = 0;
	      for(int k=0;k <  layer[i].NumberOfWeights;k++)
		{
		  getline(in,str);
		  if (str == "rnd") layer[i].neuron[j].weight[k]=(float)rand()*2/RAND_MAX-1;
		  else layer[i].neuron[j].weight[k]=strtod(str.c_str(),&strEnd);
		}				
	    }
	}
      in.close();
    }
  else return false;
  return true;
}

bool BOSS_NN::SaveNN(const string & filename)
{
  //  cout << "Entering SaveNN(" << filename << ")" << endl;
  fstream out (filename.c_str(), fstream::out);
  if (out)
    {
      out <<  NumberOfLayers << endl;
      for(int i=0;i < NumberOfLayers;i++)
	{
	  out << layer[i].NumberOfNeurons << endl;   //# of neurons in layer
	}
      for(int i=0;i < NumberOfLayers-1;i++)
	{
	  for(int j=0;j < layer[i].NumberOfNeurons;j++)
	    {
	      for(int k=0;k <  layer[i].NumberOfWeights;k++)
		{
		  out << layer[i].neuron[j].weight[k] << endl;
		}				
	    }
	}
      out.close();
    }
  else return false;
  return true;
}

float BOSS_NN::net(int l, int n, const vector<float> & Input)
{
  float nt=0,out=0,res=0;
  if(l == 0) return layer[0].neuron[n].out = Input.at(n);
  else
    {
      for(int i=0;i < layer[l-1].NumberOfNeurons;i++)
	{
	  if ((out=layer[l-1].neuron[i].out) == 0) out=net(l-1,i,Input);
	  nt += out * layer[l-1].neuron[i].weight[n];
	}
    }
  res=tanh(nt);
  layer[l].neuron[n].out=res;
  //  cout << "res " << l << "," << n << " = " << res << endl;
  return res;
}

vector<float> BOSS_NN::Calculate(const vector<float> & Input)
{
  Reset();
  //  for (int i=0;i < layer[0].NumberOfNeurons;i++) cout << Input[i] << "  ";
  vector<float> Output(layer[NumberOfLayers-1].NumberOfNeurons);
  for (int i=0;i < layer.at(NumberOfLayers-1).NumberOfNeurons;i++)
    {
      Output.at(i)=net(NumberOfLayers-1,i,Input);
    }
  return Output;
}

void BOSS_NN::Reset()
{
  for (int i=0;i < NumberOfLayers;i++)
    {
      for (int j=0;j < layer[i].NumberOfNeurons;j++)
	{
	  layer[i].neuron[j].deltaw = 0;
	  layer[i].neuron[j].out = 0;
	}
    }
}
void BOSS_NN::BackProp(vector<float> & DesiredOutput, vector<float> & Output)
{
  //cout << "Entered BackProp." << endl;
  Error=0;
  float sum;
  for (int i=0;i < layer[NumberOfLayers-1].NumberOfNeurons;i++)   //Error of Output-Layer
    {
      layer[NumberOfLayers-1].neuron[i].deltaw = Learnrate * (DesiredOutput[i] - Output[i]);
      Error += (DesiredOutput[i] - Output[i]) * (DesiredOutput[i] - Output[i]);
      //cout << (DesiredOutput[i] - Output[i]) << "  ";
    }
  Error /= 2;
  //cout << "-> " << Error << endl;
  for (int i=NumberOfLayers-2;i >= 0 ;i--)
    {
      for (int j=0;j < layer[i].NumberOfNeurons;j++)
	{
	  sum=0;
	  for (int k=0;k < layer[i+1].NumberOfNeurons; k++)
	    {
	      sum+=layer[i+1].neuron[k].deltaw * layer[i].neuron[j].weight[k];
	    }
	  layer[i].neuron[j].deltaw = (1 - layer[i].neuron[j].out * layer[i].neuron[j].out) * sum;
	}
    }
}

void BOSS_NN::Update()
{
  for (int i=NumberOfLayers-2;i >= 0 ;i--)
    {
      for (int j=0;j < layer[i].NumberOfNeurons;j++)
	{
	  for (int k=0;k < layer[i+1].NumberOfNeurons; k++)
	    {
	      layer[i].neuron[j].weight[k] += Learnrate * layer[i].neuron[j].out * layer[i+1].neuron[k].deltaw;
	    }
	}
    }
}

/** @} */ // end of boss_intonation group

