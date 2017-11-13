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

#ifndef _triangle_mesh_h_
#define _triangle_mesh_h_

#include "common.h"
#include "geometry.h"
#include "parameters_save.h"
#include "parameters_terrain.h"
#include "progress.h"
#include "random.h"
#include "triangle.h"
#include "triangle_edge.h"
#include "vertex.h"

/*! \file
  \brief Interface for class TriangleMesh.
*/

//! Contains vertices and triangles of a triangle mesh.  
/*! Abstract base class because specific classes must specify a geometry.
  Not as general-purpose as it might be due to constraints imposed by OpenGL.
  In particular, Triangle can have no attributes (e.g normal, colour) if a single 
  OpenGL call is to be made to draw all triangles, so this information is entirely associated with Vertex.
  Two colours can be associated with each vertex 
  (required for fracplanet application to obtain sharp coastlines),
  and it it a requirement for subclasses to sort triangles so that all 
  those before _triangle_switch_colour use vertex colour index 0,
  and those afterwards vertex colour index 1.
  \todo The geometry() method is a mess.  
  It would surely be better to have a Geometry* in the base class passed in via the constructor.
*/
class TriangleMesh
{
public:

  //! Constructor.
  TriangleMesh(Progress* progress);
  
  //! Destructor.
  virtual ~TriangleMesh();

  //! Accessor
  void set_emissive(float e)
    {
      _emissive=e;
    }

  //! Accessor
  float emissive() const
    {
      return _emissive;
    }

  //! Append a vertex.
  void add_vertex(const Vertex& v)
    {
      _vertex.push_back(v);
    }

  //! Append a triangle.
  void add_triangle(const Triangle& t)
    {
      _triangle.push_back(t);
    }

  //! Accessor.
  const Vertex& vertex(uint i) const
    {
      return _vertex[i];
    }

  //! Accessor.
  const Triangle& triangle(uint i) const
    {
      return _triangle[i];
    }

  //! Access the geometry of this class (needed to abstract concepts like "mid-point" and "height").
  virtual const Geometry& geometry() const
    =0;

  //! Return height of a vertex.
  float vertex_height(uint i) const
    {
      return geometry().height(vertex(i).position());
    }

  //! Set height of a vertex.
  void set_vertex_height(uint i,float h)
    {
      XYZ p(vertex(i).position());
      geometry().set_height(p,h);   
      vertex(i).position(p);
    }

  //! Return minimum height of a triangle's vertices.
  float triangle_height_min(uint i) const
    {
      const Triangle& t=triangle(i);
      return minimum
	(
	 vertex_height(t.vertex(0)),
	 vertex_height(t.vertex(1)),
	 vertex_height(t.vertex(2))
	 );
    }

  //! Return maximum height of a triangle's vertices.
  float triangle_height_max(uint i) const
    {
      const Triangle& t=triangle(i);
      return maximum
	(
	 vertex_height(t.vertex(0)),
	 vertex_height(t.vertex(1)),
	 vertex_height(t.vertex(2))
	 );
    }

  //! Return mean height of a triangle's vertices.
  float triangle_height_average(uint i) const
    {
      const Triangle& t=triangle(i);
      return 
	(
	 vertex_height(t.vertex(0))
	 +vertex_height(t.vertex(1))
	 +vertex_height(t.vertex(2))
	 )/3.0;
    }

  //! Compute and return the normal to a triangle
  const XYZ triangle_normal(uint i) const;

  //! Return which vertex colour to use for a triangle.
  uint which_colour_for_triangle(uint t) const
    {
      return (t<_triangle_switch_colour ? 0 : 1);
    }

  //! Returns number of vertices in mesh.
  uint vertices() const
    {
      return _vertex.size();
    }
  //! Returns number of triangles in mesh.
  uint triangles() const
    {
      return _triangle.size();
    }

  //! Returns number of triangles in mesh indexing colour[0] of vertices.
  uint triangles_of_colour0() const
    {
      return _triangle_switch_colour;
    }

  //! Returns number of triangles in mesh indexing colour[1] of vertices.
  uint triangles_of_colour1() const
    {
      return triangles()-_triangle_switch_colour;
    }

  //! (Re-)computes vertex normals.
  void compute_vertex_normals();

  //! Perform a single subdivision pass with perturbations up to the specified size
  void subdivide(const XYZ& variation,uint level,uint levels);
  
  //! Perform a number of subdivisions, possibly some unperturbed ("flat"), and halving the perturbation variation each iteration.
  void subdivide(uint subdivisions,uint flat_subdivisions,const XYZ& variation);

  //! Dump the mesh to the file in a form suitable for use by POVRay.
  void write_povray(std::ofstream& out,bool exclude_alternate_colour,bool double_illuminate,bool no_shadow) const;

  //! Dump the mesh to the file in a form suitable for use by Blender.
  void write_blender(std::ofstream& out,const std::string& mesh_name,const FloatRGBA* fake_alpha) const;

 protected:

  //! The vertices of this mesh.
  std::vector<Vertex> _vertex;

  //! The triangles of this mesh.
  std::vector<Triangle> _triangle; 

  //! The index of the triangle at which we switch to the alternate colour.
  uint _triangle_switch_colour;

  //! The emission level for vertices with the _emissive flag set
  float _emissive;

  //! Pointer to the progress object to which progress reports should be made.
  Progress*const _progress;
    
  //! Accessor.
  Vertex& vertex(uint i)
    {
      return _vertex[i];
    }
  
  //! Accessor.
  Triangle& triangle(uint i)
    {
      return _triangle[i];
    }
  
  //! Convenience wrapper with null test.
  void progress_start(uint steps,const std::string& info) const;

  //! Convenience wrapper with null test.
  void progress_stall(const std::string& reason) const;

  //! Convenience wrapper with null test.
  void progress_step(uint step) const;

  //! Convenience wrapper with null test.
  void progress_complete(const std::string& info) const;

 private:

  //! Fake per-vertex alpha for Blender.
  static ByteRGBA blender_alpha_workround(const ByteRGBA*,const ByteRGBA&);
};

//! A single triangle lying in the z-plane.
class TriangleMeshFlat : virtual public TriangleMesh
{
 public:

  //! Constructor.
  TriangleMeshFlat(ParametersObject::ObjectType obj,float z,uint seed,Progress* progress);

  //! Destructor.
  ~TriangleMeshFlat()
    {}  

  //! Returns the specific geometry.
  virtual const Geometry& geometry() const
    {
      return _geometry;
    }

 private:

  //! The specifc geometry for this mesh.
  GeometryFlat _geometry;
};

//! An icosahedron.
class TriangleMeshIcosahedron : virtual public TriangleMesh
{
 public:

  //! Constructor.
  TriangleMeshIcosahedron(float radius,uint seed,Progress* progress);

  //! Destructor.
  ~TriangleMeshIcosahedron()
    {}

  //! Returns the specific geometry.
  virtual const Geometry& geometry() const
    {
      return _geometry;
    }

 private:

  //! The specifc geometry for this mesh.
  GeometrySpherical _geometry;
};

//! A subdivided icosahedron.
class TriangleMeshSubdividedIcosahedron : public TriangleMeshIcosahedron
{
 public:

  //! Constructor.
  TriangleMeshSubdividedIcosahedron(float radius,uint subdivisions,uint flat_subdivisions,uint seed,const XYZ& variation,Progress* progress);

  //! Destructor.
  ~TriangleMeshSubdividedIcosahedron()
    {}
};

#endif
