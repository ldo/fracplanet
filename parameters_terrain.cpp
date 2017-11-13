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

#include "parameters_terrain.h"

/*! \todo: Might be better (more portable) to use QTime::currentTime () for random seeds.
 */
ParametersTerrain::ParametersTerrain()
  :ParametersObject()
   ,subdivisions_unperturbed(1)
   ,variation(0.0,0.0,0.125)
   ,noise(0)
   ,base_height(0)
   ,power_law(1.5)
   ,snowline_equator(0.8)
   ,snowline_pole(-0.1)
   ,snowline_power_law(1.0)
   ,snowline_slope_effect(1.0)
   ,snowline_glacier_effect(0.1)
   ,rivers(0)
   ,rivers_seed(time(0))
   ,lake_becomes_sea(0.05)
   ,oceans_and_rivers_emissive(0.0)
   ,colour_ocean    (0.0,0.0,1.0,1.0)
   ,colour_river    (0.0,0.0,1.0,1.0)
   ,colour_shoreline(1.0,1.0,0.0,1.0)
   ,colour_low      (0.0,1.0,0.0,1.0)
   ,colour_high     (1.0,0.5,0.0,1.0)
   ,colour_snow     (1.0,1.0,1.0,1.0)
{}

ParametersTerrain::~ParametersTerrain()
{}
