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
  \brief Interface for class ParametersCloud.
*/

#ifndef _parameters_cloud_h_
#define _parameters_cloud_h_

#include "common.h"
#include "parameters_object.h"
#include "rgb.h"
#include "xyz.h"

//! This class aggregates the controllable parameters for all things related to cloud generation.
class ParametersCloud : public ParametersObject
{
public:

  //! Constructor sets up some hopefully sensible defaults.
  ParametersCloud();

  //! Destructor
  ~ParametersCloud();

  //! Whether clouds will be generated.
  bool enabled;
    
  //! Height of base of clouds
  float cloudbase;

  //! Number of weather systems
  uint weather_systems;

  //! Colour for clouds
  FloatRGBA colour;

};

#endif
