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
  \brief Interface for class TriangleMeshTerrain and derived classes.
*/

#ifndef _triangle_mesh_terrain_h_
#define _triangle_mesh_terrain_h_

#include <set>

#include "common.h"
#include "image.h"
#include "parameters_terrain.h"
#include "triangle_mesh.h"

//! This class holds all the terrain-related methods.  
/*! It's intended to be used as a "mix-in", adding terrain generating 
  functionality to terrain objects subclassed from simpler geometries.
  \todo Ugh!!!  This is really yucky use of multiple inheritance.  Better for these terrain types to have-a TriangleMesh.
 */
class TriangleMeshTerrain : virtual public TriangleMesh
{
 public:

  //! Constructor.
  TriangleMeshTerrain(Progress* progress);

  //! Destructor.
  ~TriangleMeshTerrain();

  //! Dump the model as a POV scene.
  /*! Virtual method because spherical and flat terrains need e.g different sea-level planes and atmosphere layers.
   */
  virtual void write_povray(std::ofstream& out,const ParametersSave&,const ParametersTerrain&) const
    =0;

  //! Dump the model for Blender.
  /*! Unlike write_povray there are no specialisations for flat/spherical terrain.
   */
  virtual void write_blender(std::ofstream& out,const ParametersSave&,const ParametersTerrain&,const std::string& mesh_name) const;

  //! Render the mesh onto raster images (colour texture, and optionally 16-bit DEM and/or normal map).
  virtual void render_texture(Raster<ByteRGBA>&,Raster<ushort>*,Raster<ByteRGBA>*,bool shading,float ambient,const XYZ& illumination) const;

 protected:

  //! Indices of the set of triangles with all vertices at sea-level
  std::set<uint> sea_triangles;  

  //! Indices of the set of vertices comprising the river network
  std::set<uint> river_vertices; 

  //! Maximum height of terrain (used to scale to/from "normalised" height).
  float max_height;

  //! Add noise to the terrain
  void do_noise(const ParametersTerrain& parameters);

  //! Impose a sea level (raise lower vertices, and note sea triangles).
  void do_sea_level(const ParametersTerrain& parameters);

  //! Apply power law
  void do_power_law(const ParametersTerrain& parameters);

  //! Generate river network.
  void do_rivers(const ParametersTerrain& parameters);

  //! Final colouration pass.
  void do_colours(const ParametersTerrain& parameters);

  //! Invokes all the above steps (sea-level through final colouring) on a pre-subdivided triangle mesh.
  void do_terrain(const ParametersTerrain& parameters);
};

//! Class constructing specific case of a planetary terrain.
class TriangleMeshTerrainPlanet : public TriangleMeshSubdividedIcosahedron, virtual public TriangleMeshTerrain
{
 public:

  //! Constructor.
  TriangleMeshTerrainPlanet(const ParametersTerrain& param,Progress* progress);

  //! Destructor.
  ~TriangleMeshTerrainPlanet()
    {}

  //! Specifc dump-to-povray for planet terrain.
  void write_povray(std::ofstream& out,const ParametersSave&,const ParametersTerrain&) const;
};

//! Class constructing specific case of a flat-base terrain area.
class TriangleMeshTerrainFlat : public TriangleMeshFlat, virtual public TriangleMeshTerrain
{
 public:
  
  //! Constructor.
  TriangleMeshTerrainFlat(const ParametersTerrain& parameters,Progress* progress);

  //! Destructor.
  ~TriangleMeshTerrainFlat()
    {}

  //! Specifc dump-to-povray for flat terrain area.
  void write_povray(std::ofstream& out,const ParametersSave&,const ParametersTerrain&) const;
};

#endif
