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
  \brief Interface for class ParametersNoise.
*/

#ifndef _parameters_noise_h_
#define _parameters_noise_h_

#include "common.h"

class ParametersNoise
{
 public:

  //! Construct with given number of terms
  ParametersNoise(uint);

  //! Destructor
  ~ParametersNoise();

  //! Number of Perlin noise terms
  uint terms;

  //! Frequency of 1st noise term
  float frequency;

  //! Amplitude of 1st noise term
  float amplitude;

  //! Amplitude decay rate for successive terms
  float amplitude_decay;
};

#endif
