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

#include "triangle_mesh_cloud.h"

#include "noise.h"
#include "matrix34.h"
#include "parameters_render.h"

TriangleMeshCloud::TriangleMeshCloud(Progress* progress)
  :TriangleMesh(progress)
{}

TriangleMeshCloud::~TriangleMeshCloud()
{}

void TriangleMeshCloud::write_povray(std::ofstream& out,const ParametersSave&,const ParametersCloud&) const
{
  // Double illuminate so underside of clouds is white.
  // No-shadow so clouds don't cast crazy dark shadows.
  TriangleMesh::write_povray(out,false,true,true);
}

void TriangleMeshCloud::write_blender(std::ofstream& out,const ParametersSave& parameters_save,const ParametersCloud&,const std::string& mesh_name) const
{
  TriangleMesh::write_blender
    (
     out,
     mesh_name+".cloud",
     (parameters_save.blender_per_vertex_alpha ? 0 : &parameters_save.parameters_render->background_colour_low)
     );
}

namespace 
{
  class ScanConvertHelper : public ScanConvertBackend
  {
  public:
    ScanConvertHelper(Raster<uchar>& image,const boost::array<float,3>& vertex_colours)
      :ScanConvertBackend(image.width(),image.height())
      ,_image(image)
      ,_vertex_colours(vertex_colours)
    {}
    virtual ~ScanConvertHelper()
    {}
    virtual void scan_convert_backend(uint /*y*/,const ScanEdge& /*edge0*/,const ScanEdge& /*edge1*/) const
    {}
    virtual void subdivide(const boost::array<XYZ,3>& /*v*/,const XYZ& /*m*/,const ScanConverter& /*scan_converter*/) const
    {}
  private:
    Raster<uchar>& _image;
    const boost::array<float,3>& _vertex_colours;
  };
}

void TriangleMeshCloud::render_texture(Raster<uchar>& image) const
{
  assert(false);
  image.fill(0);
  for (uint i=0;i<triangles();i++)
    {
      const Triangle& t=triangle(i);
      const boost::array<XYZ,3> vertex_positions
	={{
	  vertex(t.vertex(0)).position(),
	  vertex(t.vertex(1)).position(),
	  vertex(t.vertex(2)).position()
	}};
      const boost::array<float,3> vertex_colours
	={{
	  FloatRGBA(vertex(t.vertex(0)).colour(0)).a,
	  FloatRGBA(vertex(t.vertex(1)).colour(0)).a,
	  FloatRGBA(vertex(t.vertex(2)).colour(0)).a
	}};

      ScanConvertHelper scan_convert_backend(image,vertex_colours);
      geometry().scan_convert
	(
	 vertex_positions,
	 scan_convert_backend
	 );
    }
}

void TriangleMeshCloud::do_cloud(const ParametersCloud& parameters)
{
  compute_vertex_normals();

  progress_start(100,"Cloud colouring");

  const ByteRGBA c(parameters.colour);

  //! \todo Wire up terms, decay and base fequency and thresholds
  MultiscaleNoise noise(parameters.seed,6,0.5);
  for (uint i=0;i<vertices();i++)
    {
      progress_step((100*i)/vertices());

      const float v=0.5+0.5*noise(4.0f*vertex(i).position());
      const float v_min=0.5f;
      const float v_max=0.6f;
      const float v_k=1.0f/(v_max-v_min);
      const float vs=std::min(1.0f,std::max(0.0f,(v-v_min)*v_k));

      vertex(i).colour(0,ByteRGBA(c.r,c.g,c.b,static_cast<uint>(255.0*vs)));

      // Set other colour (unused) to red for debug
      vertex(i).colour(1,ByteRGBA(255,0,0,255));
    }
  progress_complete("Cloud colouring completed");

  // TODO: Eliminate all-transparent triangles & unused vertices.
  // Leave if nothing left

  // TODO: Bias weather into temperate bands (maybe not)

  progress_start(100,"Weather systems");

  Random01 r01(parameters.seed);
  const uint steps=100*vertices();
  uint step=0;
  for (uint i=0;i<parameters.weather_systems;i++)
    {
      const uint random_vertex=static_cast<uint>(r01()*vertices());
      const XYZ position(vertex(random_vertex).position());
      const XYZ axis(geometry().up(position));

      // Rotate opposite direction in other hemisphere
      const float strength=r01()*(position.z<0.0 ? -M_PI : M_PI);

      for (uint j=0;j<vertices();j++)
	{
	  progress_step((100*step)/steps);
	  step++;
      
	  const XYZ p(vertex(j).position());
	  const XYZ pn=geometry().up(p);
	  const float pna=pn%axis;

	  if (pna>0.0f)  // Don't create same feature on other side of planet (actually the distance would be big so could drop this)
	    {
	      const float distance=(p-position).magnitude();
	      const float rotation_angle=strength*exp(-10.0*distance);
	      
	      // Now rotate p about axis through position by the rotation angle
	      // TODO: Optimise!  axis and position is the same for all points; we're constantly recomputing the basis change matrices.
	      // Create a stateful version of Matrix34RotateAboutAxisThrough.
	      vertex(j).position
		(
		 Matrix34RotateAboutAxisThrough(axis,rotation_angle,position)*p
		 );
	    }
	}
    }

  progress_complete("Weather systems completed");
  
  _triangle_switch_colour=triangles();
}

TriangleMeshCloudPlanet::TriangleMeshCloudPlanet(const ParametersCloud& parameters,Progress* progress)
  :TriangleMesh(progress)
  ,TriangleMeshCloud(progress)
  ,TriangleMeshSubdividedIcosahedron(1.0+parameters.cloudbase,parameters.subdivisions,parameters.subdivisions,parameters.seed,XYZ(0.0,0.0,0.0),progress)
{
  do_cloud(parameters);
}

TriangleMeshCloudFlat::TriangleMeshCloudFlat(const ParametersCloud& parameters,Progress* progress)
  :TriangleMesh(progress)
  ,TriangleMeshCloud(progress)
  ,TriangleMeshFlat(parameters.object_type,parameters.cloudbase,parameters.seed,progress)
{
  subdivide(parameters.subdivisions,parameters.subdivisions,XYZ(0.0,0.0,0.0));
  do_cloud(parameters);
}

