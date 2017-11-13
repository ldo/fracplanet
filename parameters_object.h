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
  \brief Interface for class Parameters.
*/

#ifndef _parameters_h_
#define _parameters_h_

#include "common.h"

//! Common base for ParametersTerrain and ParametersCloud.
class ParametersObject
{
 public:

  //! Constructor sets up some hopefully sensible defaults.
  ParametersObject();
  
  //! Destructor
  virtual ~ParametersObject();
  
  //! What kind of object will be generated.
  typedef enum
    {
      ObjectTypePlanet,
      ObjectTypeFlatHexagon,
      ObjectTypeFlatTriangle,
      ObjectTypeFlatSquare
    }
  ObjectType;

  //! Kind of object.
  ObjectType object_type;

  //! Random seed for subdivision and noise.
  uint seed;

  //! Number of subdivisions.
  uint subdivisions;
};

#endif
