/* dsp_fft.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 1997-2001 University of Bonn.
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
   A class for computing the fft for a vector. The computation is table based.
   @author Wolfgang Hess
   @author Karlheinz Stoeber
*/

#ifdef SWAP
#undef SWAP
#endif
#define SWAP(a,b) x=(a);(a)=(b);(b)=x;

#ifndef log2
#define log2(x)   (log(x)/M_LN2)
#endif

template < class T >
void fft<T>::mbtrbtr (T * b, T * b1, int * ispieg, int * ispg1) const
{
  register int i, j, k;
  register T       x;
  
  for (i=1; i<ispieg[0]; i+=2) {
    j = ispieg[i];
    k = ispg1 [i];
    SWAP(b[k],  b[j]);
    SWAP(b1[k], b1[j]);
  }
}

template < class T >
void fft<T>::mshfbgr (const int n, T * b) const
{
  register int  i, j, k=1;
  register T        x;
      
  for (i=j=3; i<n; i+=2) {
    if (j > i) {
      SWAP(b[i], b[j]);
    }
    j -= 2;
    if (j <= k) {
      j = 2 * (i + 1) - 1;
      k = i;
    }
  }
}

template < class T >
void fft<T>::mff2ti (const int iii, int nn, T * b0, T * b1, T * b2) const
{
  register T       zr, zi, c1, s1;
  register int i, j, k, j0;
  int ii   = 7;
  int il   = 4;
  int ir   = 4;
  int iii2 = iii << 1;
      
  nn >>= 1;
  for (i=0; (i<iii2) && (nn!=0); i++) {
    zr    = b0[i] + b2[i];
    b2[i] = b0[i] - b2[i];
    b0[i] = zr;
  }

  c1 = cosinus[1];
  k  = (iii << 2) + iii;
  for (j=(i=k-iii)+iii2; (i<k) && (nn>1); i++, j++) {
    zr    =  b0[i] - b0[j];
    zi    =  b1[i] + b1[j];
    b0[i] =  b0[i] + b0[j];
    b0[j] =  b1[j] - b1[i];
    b1[i] = (zr + zi) * c1;
    b1[j] = (zi - zr) * c1;
  }
      
  for (i=2; i<nn; i++) {
    c1 = cosinus[i];
    s1 = sinus[i];
    j  = (j0 = ir * iii2 + iii) - iii;
    for (k=ii*iii2; j<j0; j++, k++) {
      zr    = b0[j] - b0[k];
      zi    = b1[j] + b1[k];
      b0[j] = b0[j] + b0[k];
      b0[k] = b1[k] - b1[j];
      b1[j] = zr * c1 - zi * s1;
      b1[k] = zr * s1 + zi * c1;
    }
    ir += 2;
    ii -= 2;
    if (ii <= il) {
      ii = (ir << 1) - 1;
      il =  ir;
    }
  }
}

template < class T >
void fft<T>::mff2tr (const int iii, int nn, T * b0, T * b1, T * b2) const
{
  register T       zr, zi, c1, s1;
  register int i, j, k, j0;
  int ii   = 7;
  int il   = 4;
  int ir   = 4;
  int iii2 = iii << 1;

  nn >>= 1;
  for (i=0; (i<iii2) && (nn!=0); i++) {
    zr    = b0[i] + b2[i];
    b2[i] = b0[i] - b2[i];
    b0[i] = zr;
  }

  c1 = cosinus[1];
  k  = (iii << 2) + iii;
  for (j=(i=k-iii)+iii2; (i<k) && (nn>1); i++, j++) {
    zr    = (b1[i] - b1[j]) * c1;
    zi    = (b1[i] + b1[j]) * c1;
    b1[j] =  b0[j] + zi;
    b1[i] =  zi    - b0[j];
    b0[j] =  b0[i] - zr;
    b0[i] =  b0[i] + zr;
  }

  for (i=2; i<nn; i++) {
    c1 = cosinus[i];
    s1 = sinus[i];
    j  = (j0 = ir * iii2 + iii) - iii;
    for (k=ii*iii2; j<j0; j++, k++) {
      zr    = b1[j] * c1 + b1[k] * s1;
      zi    = b1[k] * c1 - b1[j] * s1;
      b1[j] = zi    - b0[k];
      b1[k] = b0[k] + zi;
      b0[k] = b0[j] - zr;
      b0[j] = b0[j] + zr;
    }
    ir += 2;
    ii -= 2;
    if (ii <= il) {
      ii = (ir << 1) - 1;
      il = ir;
    }
  }
}

template < class T >
int fft<T>::mffir2 (int n, T * b) const
{
  register int  i, ldn;
  register T        x;

  for (i=3; i<n; i+=2)
    b[i] = -b[i];

  x = 1.0 / T(n >> 1);

  for (i=0; i<n; i++)
    b[i] *= x;

  mbtrbtr(b, b+1, mirror, mirror+1);
  mshfbgr(n, b);

  x    = (b[0] + b[1]) * 0.5;
  b[1] = (b[0] - b[1]) * 0.5;
  b[0] =  x;

  for (i=1, ldn=n>>1; i<ldn; i<<=1)
    mff2ti (i, (n>>=1), b, b+i, b+(i<<1));

  return (i<<1);
}

template < class T >
int fft<T>::mfftr2 (const int n, T * b) const
{
  register int i, j;
  register T       x;
      
  for (i=n>>2, j=1; i>0; i>>=1)
    mff2tr(i, (j<<=1), b, b+i, b+(i<<1));
      
  x    = b[0] + b[1];
  b[1] = b[0] - b[1];
  b[0] = x;
      
  mshfbgr(n, b);
  mbtrbtr(b, b+1, mirror, mirror+1);
      
  for (i=3; i<n; i+=2)
    b[i] = -b[i];
      
  return (j<<1);
}

template < class T >
int fft<T>::mtabbr2 (int ldn, int mode) const
{
  register int    i, j, k, n, ldn2;
  register double     x, y;
      
  ldn2 = max((ldn-2), 1);
  n = 1 << ldn;
      
  if (mode <= 1) {
    x = (-2.0 * M_PI) / n;
    for (i=0, n>>=2; i<n; i++) {
      j          = mbitrev (ldn2, i);
      y          = x * i;
      sinus[j]   = sin(y);
      cosinus[j] = cos(y);
    }
    n<<=1;
  }
      
  ldn--;
  mirror[0] = 0;
  for (i=0, k=1; i<n; i++) {
    j = mbitrev (ldn, i);
    if (j>i) {
      mirror[k]   = i << 1;
      mirror[k+1] = j << 1;
      mirror[0]   = ++k;
      k++;
    }
  }
  return (n<<1);
}

template < class T >
unsigned fft<T>::mbitrev (unsigned ldn, unsigned inval) const
{
  register unsigned iu  = 0;
  register unsigned iii = 1;
  register unsigned i   = ldn;

  static unsigned br [] = { // Powers of two
    0x0,       0x1,        0x2,        0x4,
      0x8,       0x10,       0x20,       0x40,
      0x80,      0x100,      0x200,      0x400,
      0x800,     0x1000,     0x2000,     0x4000,
      0x8000,    0x10000,    0x20000,    0x40000,
      0x80000,   0x100000,   0x200000,   0x400000,
      0x800000,  0x1000000,  0x2000000,  0x4000000,
      0x8000000, 0x10000000, 0x20000000, 0x40000000
      };

  while (i>0) {
    if ((inval&iii)>0)
      iu = iu + br[i]; // tables approx. 30% faster than (1<<i) !!!
    iii += iii;
    i--;
  }

  return iu;
}

template < class T >
fft<T>::fft(const unsigned & s) : Size(s)
{
  sinus   = new double  [Size];
  cosinus = new double  [Size];
  mirror  = new int     [Size];
  mtabbr2 ((int)log2((double)Size), 0);
}

template < class T >
fft<T>::~fft()
{
  if(Size>0) {
    delete [] sinus;
    delete [] cosinus;
    delete [] mirror;
  }
}

template < class T >
vector< T > fft<T>::operator () (const vector< T > & data, const FFTType flag) const
{
  vector< T >res(data);
  if(flag == FORWARD)
    mfftr2 (res.Size, res.Data);
  else
    mffir2 (res.Size, res.Data);
  return res;
}

#undef SWAP
