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
  \brief Interface for class ParametersSave.
*/

#ifndef _parameters_save_h_
#define _parameters_save_h_

#include "common.h"

class ParametersRender;

//! Aggregates controllable parameters for all things related to save.
class ParametersSave
{
 public:

  //! Constructor.
  ParametersSave(const ParametersRender*);

  //! Destructor.
  ~ParametersSave();

  //! Whether to emit an atmosphere object to POV file.
  bool pov_atmosphere;

  //! Whether to emit a single sea-level object to POV file.
  bool pov_sea_object;

  //! Whether to try using per-vertex-alpha in the blender output.
  bool blender_per_vertex_alpha;

  //! Whether textures should include shading.
  bool texture_shaded;
  
  //! Size of texture for texture save (is height; width is implicit).
  uint texture_height;

  //! Save for blender needs access to some of these.
  const ParametersRender*const parameters_render;
};

#endif
