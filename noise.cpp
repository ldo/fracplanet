/**************************************************************************/
/*  Copyright 2009 Tim Day                                                */
/*                                                                        */
/*  This file is part of Fracplanet                                       */
/*                                                                        */
/*  Fracplanet is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Fracplanet is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Fracplanet.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file
  \brief Implementation of Noise class
*/

#include "noise.h"

Noise::Noise(uint seed)
{
  // We use our own random number generator so saved pictures will be the same when reloaded!
  // (Besides, one isn't actually conveniently available)
  Random01 r_01(seed);

  // Create an array of random gradient vectors uniformly on the unit sphere

  unsigned int i;
  
  for (i=0;i<N;i++)
    _g[i]=RandomXYZSphereNormal(r_01);
  
  // Create a pseudorandom permutation of [1..B] 
  
  for (i=0;i<=N;i++)
    _p[i]=i;
  
  for (i=N;i>0;i-=2) 
    {
      const int j=(static_cast<int>(r_01()*N));
      int k=_p[i];
      _p[i]=_p[j];
      _p[j]=k;
    }
  
  // Extend g and p arrays to allow for faster indexing
  
  for (i=0;i<N+2;i++)
    {
      _p[N+i]=_p[i];
      _g[N+i]=_g[i];
    }
}

Noise::~Noise()
{}

inline float value(const XYZ& q,float rx,float ry,float rz)
{
  return rx*q.x+ry*q.y+rz*q.z;
}

inline float surve(float t)
{
  return t*t*(3.0-2.0*t);
}

inline float lerp(float t,float a,float b)
{
  return a+t*(b-a);
}

float Noise::operator()(const XYZ& p) const
{
  // Crank up the frequency a bit otherwise don't see much variation in base case
  const float tx=2.0*p.x+10000.0f;
  const float ty=2.0*p.y+10000.0f;
  const float tz=2.0*p.z+10000.0f;
  
  const int itx=static_cast<int>(tx);
  const int ity=static_cast<int>(ty);
  const int itz=static_cast<int>(tz);
  
  const int bx0=( itx   &(N-1));
  const int bx1=((bx0+1)&(N-1));
  const int by0=( ity   &(N-1));
  const int by1=((by0+1)&(N-1));
  const int bz0=( itz   &(N-1));
  const int bz1=((bz0+1)&(N-1));
  
  const int i=_p[bx0];
  const int b00=_p[i+by0];
  const int b01=_p[i+by1];
  
  const int j=_p[bx1];
  const int b10=_p[j+by0];
  const int b11=_p[j+by1];

  const float rx0=tx-itx;
  const float ry0=ty-ity;
  const float rz0=tz-itz;
  
  const float rx1=rx0-1.0;
  const float ry1=ry0-1.0;
  const float rz1=rz0-1.0;
    
  const float sx=surve(rx0);
  
  const float a0=lerp(sx,value(_g[b00+bz0],rx0,ry0,rz0),value(_g[b10+bz0],rx1,ry0,rz0));
  const float b0=lerp(sx,value(_g[b01+bz0],rx0,ry1,rz0),value(_g[b11+bz0],rx1,ry1,rz0));
  const float a1=lerp(sx,value(_g[b00+bz1],rx0,ry0,rz1),value(_g[b10+bz1],rx1,ry0,rz1));
  const float b1=lerp(sx,value(_g[b01+bz1],rx0,ry1,rz1),value(_g[b11+bz1],rx1,ry1,rz1));
  
  const float sy=surve(ry0);
  
  const float c=lerp(sy,a0,b0);  
  const float d=lerp(sy,a1,b1);
  
  const float sz=surve(rz0);
  
  return 1.5*lerp(sz,c,d);
}

MultiscaleNoise::MultiscaleNoise(uint seed,uint terms,float decay)
  :_terms(terms)
  ,_noise(new boost::scoped_ptr<const Noise>[terms])
  ,_amplitude(new float[terms])
{
  float k=1.0f;
  float kt=0.0f;
  for (uint i=0;i<_terms;i++)
    {
      _noise[i].reset(new Noise(seed+i));
      _amplitude[i]=k;
      kt+=k;
      k*=decay;
    }
  for (uint i=0;i<_terms;i++)
    {
      _amplitude[i]/=kt;
    }
}

MultiscaleNoise::~MultiscaleNoise()
{}

//! Return noise value at a point.
float MultiscaleNoise::operator()(const XYZ& p) const
{
  float v=0.0;
  for (uint i=0;i<_terms;i++)
    {
      v+=_amplitude[i]*(*_noise[i])(p*(1<<i));
    }
  return v;
}
