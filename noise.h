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
  \brief Interface to Noise class.
*/

#ifndef _noise_h_
#define _noise_h_

#include "common.h"
#include "xyz.h"

//! Perlin noise generator.
class Noise
{
public:

  //! Constructor.
  Noise(uint seed);

  //! Destructor.
  ~Noise();

  //! Return noise value at a point.
  float operator()(const XYZ& p) const;

protected:

  //! Number of table entries.
  enum {N=256};
  
  int _p[2*N+2];
  XYZ _g[2*N+2];
};

//! Multiscale noise generator.
class MultiscaleNoise
{
 public:

  //! Constructor.
  MultiscaleNoise(uint seed,uint terms,float decay);

  //! Destructor.
  ~MultiscaleNoise();

  //! Return noise value at a point.
  float operator()(const XYZ& p) const;

 private:

  //! Number of terms
  const uint _terms;

  //! Noise functions for each frequency.
  boost::scoped_array<boost::scoped_ptr<const Noise> > _noise;

  //! Amplitude for each frequency.
  boost::scoped_array<float> _amplitude;
};

#endif
