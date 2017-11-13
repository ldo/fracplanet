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
  \brief Interface for class ParametersTerrain.
*/

#ifndef _parameters_terrain_h_
#define _parameters_terrain_h_

#include "common.h"
#include "parameters_noise.h"
#include "parameters_object.h"
#include "rgb.h"
#include "xyz.h"

//! This class aggregates the controllable parameters for all things related to terrain generation.
/*! \todo Add these to ParametersTerrain (and ControlTerrain):  float treeline;  float beachline;
*/
class ParametersTerrain : public ParametersObject
{
public:

  //! Constructor sets up some hopefully sensible defaults.
  ParametersTerrain();

  //! Destructor
  ~ParametersTerrain();

  //! Numer of subdivisions (at the top level) which will be unperturbed
  uint subdivisions_unperturbed;
  
  //! Maximum size of perturbations (z in vertical direction, x & y horizontally).
  XYZ variation;

  ParametersNoise noise;

  //! Initial height of unsubdivided, unperturbed terrain, expressed as a proportion of variation.z
  float base_height;

  //! Power law applied to terrain heights.
  /*! When >1, flattens low areas.  When <1, flattens highland areas and creates gorges.
   */
  float power_law;

  //! Normalised height of snowline at the equator.
  float snowline_equator;  

  //! Normalised height of snowline at the pole.
  float snowline_pole;  

  //! Power law for snowline.
  float snowline_power_law;  

  //! Supresses snow on slopes.
  float snowline_slope_effect;  

  //! When positive, rivers become glaciers.  When negative, rivers remain blue.
  float snowline_glacier_effect;

  //! Number of rivers to generate.
  uint rivers;

  //! Random seed for river generation.
  uint rivers_seed;
  
  //! Limit on lake size as a proportion of available surface.
  /*! When lakes (produced during the river-growing step) cover this fraction of the available terrain they become seas and no-longer need to find a drain.
   */
  float lake_becomes_sea;
  
  //! Amount of emissive colour for oceans and rivers.
  float oceans_and_rivers_emissive;

  //@{
  //! Colour for a terrain type.
  FloatRGBA colour_ocean;
  FloatRGBA colour_river;
  FloatRGBA colour_shoreline;
  FloatRGBA colour_low;
  FloatRGBA colour_high;
  FloatRGBA colour_snow;
  //@}
};

#endif
