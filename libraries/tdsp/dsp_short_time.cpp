/* dsp_short_time.cpp    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/**
   A set of functions dividing a vector into a set of
   short time frames.
   @author Karlheinz Stoeber
*/

#include "dsp_short_time.h"

using namespace dsp;

template < class TM, class TV >
void shortTime(matrix< TM > & m, const vector< TV > & v, const unsigned & frame_size, const unsigned & frame_shift, const TWindowType w = RECTANGULAR)
{
  shortTime(m, v, frame_size, frame_shift, unsigned(0), v.size(), w);
}

template < class TM, class TV >
void shortTime(matrix< TM > & m, const vector< TV > & v, const unsigned & frame_size, const unsigned & frame_shift, const unsigned & left, const unsigned & right, const TWindowType w = RECTANGULAR) throw(IndexError)
{
  static const string F_Name("void shortTime(matrix< TM > &, const vector< TV > &, const unsigned &, const unsigned &, const unsigned &, const unsigned &, const TWindowType)");

  if(right<=left || right>v.size()) {
    TKVPairList kvlist;
    kvlist.push_back(TKVPair("Size",  v.size()));
    kvlist.push_back(TKVPair("Left",  left));
    kvlist.push_back(TKVPair("Right", right));
    throw IndexError(F_Name, kvlist);
  }

  int fs2 = frame_size / 2;
  int num_frames = int(rint(ceil(float(right-left)/float(frame_shift))));
  m = matrix< TM >(num_frames, frame_size);

  for(int i=left, r=0; r<num_frames; i+=frame_shift, r++) {
    int ur = i + fs2;
    if(frame_size%2)
      ur++;
    for(int j=i-fs2, c=0; j<ur; j++, c++) {
      if(j>=0 && j<int(v.size())) {
	switch(w) {
	case HANNING:
	  m(r, c) = TM(v[j] * dsp::hanning(c-fs2, frame_size));
	  break;
	case HAMMING:
	  m(r, c) = TM(v[j] * dsp::hamming(c-fs2, frame_size));
	  break;
	case RECTANGULAR:
	  m(r, c) = TM(v[j]);
	  break;
	}
      }
      else
	m(r, c) = TM(0);	
    }
  }
}
