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

#include "triangle_mesh_terrain.h"

#include <map>

#include "noise.h"

TriangleMeshTerrain::TriangleMeshTerrain(Progress* progress)
  :TriangleMesh(progress)
  ,max_height(0.0)
{}

TriangleMeshTerrain::~TriangleMeshTerrain()
{}


void TriangleMeshTerrain::do_noise(const ParametersTerrain& parameters)
{
  if (parameters.noise.terms==0 || parameters.noise.amplitude==0) return;

  const uint steps=vertices();
  uint step=0;

  progress_start(100,"Noise");

  MultiscaleNoise noise(parameters.seed,parameters.noise.terms,parameters.noise.amplitude_decay);
  for (uint i=0;i<vertices();i++)
    {
      step++;
      progress_step((100*step)/steps);	  
      
      const float h=vertex_height(i);
      const float p=parameters.noise.amplitude*noise(parameters.noise.frequency*vertex(i).position());
      
      set_vertex_height(i,h+p);
    }
  
  progress_complete("Noise complete");
}

void TriangleMeshTerrain::do_sea_level(const ParametersTerrain& /*parameters*/)
{
  // Introduce sea level  
  const uint steps=vertices()+triangles();
  uint step=0;
  
  progress_start(100,"Sea level");
  {    
    std::vector<bool> sea_vertices(vertices());    // Flag vertices forced to sea level
    
    for (uint i=0;i<vertices();i++)
      {
	step++;
	progress_step((100*step)/steps);
	
	const float m=vertex_height(i);
	
	if (m<=0.0)
	  {
	    set_vertex_height(i,0.0);
	    sea_vertices[i]=true;
	  }
	else if (m>max_height)
	  max_height=m;
      }

    std::vector<Triangle> land_sea[2];
   
    for (uint i=0;i<triangles();i++)
      {
	step++;
	progress_step((100*step)/steps);

	const Triangle& t=triangle(i);
	land_sea[sea_vertices[t.vertex(0)] && sea_vertices[t.vertex(1)] && sea_vertices[t.vertex(2)]].push_back(t);
      }

    _triangle_switch_colour=land_sea[0].size();
    _triangle=land_sea[0];
    _triangle.insert(_triangle.end(),land_sea[1].begin(),land_sea[1].end());
  }
  progress_complete("Sea level completed");
}

void TriangleMeshTerrain::do_power_law(const ParametersTerrain& parameters)
{
  const uint steps=vertices();
  uint step=0;
  
  progress_start(100,"Power law");
  
  for (uint i=0;i<vertices();i++)
    {
      step++;
      progress_step((100*step)/steps);

      const float h=vertex_height(i);
      if (h>geometry().epsilon())
	set_vertex_height(i,max_height*pow(h/max_height,parameters.power_law));
    }

  progress_complete("Power law completed");
}

inline void insert_unique(std::vector<uint>& v,uint x)
{
  if (std::find(v.begin(),v.end(),x)==v.end()) v.push_back(x);
}

void TriangleMeshTerrain::do_rivers(const ParametersTerrain& parameters)
{
  if (parameters.rivers==0) return;
  
  Random01 r01(parameters.rivers_seed);
  
  std::vector<bool> is_sea_vertex(vertices());

  // Used to use a vector of sets, but this saves memory and there aren't many elements to each set
  std::vector<std::vector<uint> > vertex_neighbours(vertices());  
  
  progress_start(100,"River preparation");
  
  const uint prep_steps=triangles();
  uint step=0;
  
  // Need to know each vertex's neighbours (for land vertices)
  for (uint i=0;i<triangles_of_colour0();i++)
    {
      step++;
      progress_step((100*step)/prep_steps);
      
      const Triangle& t=triangle(i);
      insert_unique(vertex_neighbours[t.vertex(0)],t.vertex(1));
      insert_unique(vertex_neighbours[t.vertex(0)],t.vertex(2));
      insert_unique(vertex_neighbours[t.vertex(1)],t.vertex(0));
      insert_unique(vertex_neighbours[t.vertex(1)],t.vertex(2));
      insert_unique(vertex_neighbours[t.vertex(2)],t.vertex(0));
      insert_unique(vertex_neighbours[t.vertex(2)],t.vertex(1));
    }

  for (uint i=triangles_of_colour0();i!=triangles();i++)
    {
      step++;
      progress_step((100*step)/prep_steps);
      
      const Triangle& t=triangle(i);
      is_sea_vertex[t.vertex(0)]=true;
      is_sea_vertex[t.vertex(1)]=true;
      is_sea_vertex[t.vertex(2)]=true;
    }
  
  progress_complete("River preparation completed");

  progress_start(100,"Rivers");
  
  const uint maximum_lake_size=(uint)(vertices()*parameters.lake_becomes_sea);
  
  const uint steps=parameters.rivers;
  step=0;
  
  for (uint r=0;r<parameters.rivers;r++)
    {
      step++;
      progress_step((100*step)/steps);
      
      unsigned int last_stall_warning=0;
      
      std::set<uint> vertices_to_add;
      // Also track the height of vertices in the river.
      // When a river level rise is forced we can look for upstream points to bring back into the current set
      std::multimap<float,uint> vertices_to_add_by_height;
      
      std::set<uint> current_vertices;
      float current_vertices_height;
      
      // Pick a random non sea triangle to start river
      // Would like to use __gnu_cxx::random_sample_n but can't get it to work
      uint source_vertex=(uint)(r01()*vertices());
      
      // Rivers starting in the sea are skipped to keep river density independent of land area, and to avoid lock-up on all-sea planets
      if (is_sea_vertex[source_vertex])
	continue;
      
      current_vertices.insert(source_vertex);
      current_vertices_height=vertex_height(source_vertex);
      
      while(true)
	{
	  bool reached_sea=false;
	  std::set<uint> current_vertices_neighbours;
	  
	  for (std::set<uint>::const_iterator it=current_vertices.begin();it!=current_vertices.end();it++)
	    {
	      vertices_to_add.insert(*it);
	      vertices_to_add_by_height.insert(std::make_pair(current_vertices_height,*it));
	      reached_sea|=(is_sea_vertex[*it]);
	      
	      const std::vector<uint>& neighbours=vertex_neighbours[*it];
	      
	      // Only vertices NOT in the current river section are of interest
	      for (std::vector<uint>::const_iterator it_n=neighbours.begin();it_n!=neighbours.end();it_n++)
		{
		  if (current_vertices.find(*it_n)==current_vertices.end()) 
		    current_vertices_neighbours.insert(*it_n);
		}
	    }
	  
	  // Find the heights of everything we could flow to
	  std::multimap<float,uint> flow_candidates;
	  for (std::set<uint>::const_iterator it=current_vertices_neighbours.begin();it!=current_vertices_neighbours.end();it++)
	    {
	      flow_candidates.insert(std::multimap<float,uint>::value_type(vertex_height(*it),*it));
	    }
	  
	  if (reached_sea)
	    {
	      break;
	    }
	  else if (current_vertices.size()>=maximum_lake_size)
	    {
	      break;  // Lake becomes an inland sea
	    }
	  else
	    {
	      // If there is any intersection between the current river hexes and completed rivers then we're done
	      bool meets_existing=false;
	      for (std::set<uint>::const_iterator it=current_vertices.begin();it!=current_vertices.end();it++)
		{
		  if (river_vertices.find(*it)!=river_vertices.end())
		    {
		      meets_existing=true;
		      break;
		    }
		}
	      if (meets_existing)
		{
		  break;
		}
	    }
	  
	  unsigned int num_current_vertices=0;
	  if (flow_candidates.empty()) // Not sure how this could ever happen, but just to be safe...
	    {
	      std::cerr << "Warning: Unexpected internal state: no flow candidates for a river\n";  
	      break;
	    }
	  else if ((*(flow_candidates.begin())).first<current_vertices_height-geometry().epsilon())  // Can flow downhill...
	    {
	      // We just flow into ONE vertex, otherwise rivers will expand to inundate all lower terrain
	      current_vertices.clear();
	      current_vertices_height=(*(flow_candidates.begin())).first;
	      current_vertices.insert((*(flow_candidates.begin())).second);
	      num_current_vertices=1;
	      //std::cerr << "-" << current_vertices.size();
	    }
	  else if ((*(flow_candidates.begin())).first<current_vertices_height+geometry().epsilon())  // Or expand across flat...
	    {
	      // But if there are multiple candidates on the same level, flow into all of them
	      std::multimap<float,uint>::const_iterator flat_end=flow_candidates.upper_bound(current_vertices_height+geometry().epsilon());
	      
	      for (std::multimap<float,uint>::const_iterator it_flat=flow_candidates.begin();it_flat!=flat_end;it_flat++)
		{
		  current_vertices.insert((*it_flat).second);
		}
	      num_current_vertices=current_vertices.size();
	      //std::cerr << "=" << current_vertices.size();
	    }
	  else // Otherwise the water level must rise to height of the lowest flow candidate 
	    {
	      current_vertices_height=(*(flow_candidates.begin())).first+geometry().epsilon();   // Rise a bit more to avoid getting stuck due to precision.
	      const uint outflow_vertex=(*(flow_candidates.begin())).second;
	      current_vertices.insert(outflow_vertex);
	      
	      // Any vertices upstream below the new height should now become part of the current set, and have their level raised.
	      std::multimap<float,uint>::iterator backflow_end=vertices_to_add_by_height.upper_bound(current_vertices_height);
	      for (std::multimap<float,uint>::iterator it=vertices_to_add_by_height.begin();it!=backflow_end;it++)
		{
		  const uint v=(*it).second;
		  current_vertices.insert(v);
		  set_vertex_height(v,current_vertices_height);
		}
	      vertices_to_add_by_height.erase(vertices_to_add_by_height.begin(),backflow_end);
	      
	      // Raise level of everything in the working set
	      // Also count vertices rather than having .size() iterate over them again.
	      for (std::set<uint>::const_iterator it=current_vertices.begin();it!=current_vertices.end();it++)
		{
		  set_vertex_height(*it,current_vertices_height);
		  num_current_vertices++;
		}
	      //std::cerr << "+" << current_vertices.size();
	    }
	  
	  if (num_current_vertices>=last_stall_warning+100)
	    {
	      std::ostringstream msg;
	      msg << "Rivers (delay: " << num_current_vertices << " vertex lake)";
	      progress_stall(msg.str());
	      last_stall_warning=num_current_vertices;
	    }
	  else if (num_current_vertices+100<=last_stall_warning)
	    {
	      std::ostringstream msg;
	      msg << "Rivers: lake complete";
	      progress_stall(msg.str());
	      last_stall_warning=num_current_vertices;
	    }
	}

      river_vertices.insert(vertices_to_add.begin(),vertices_to_add.end());
    }
    
  progress_complete("Rivers completed");
}

void TriangleMeshTerrain::do_colours(const ParametersTerrain& parameters)
{
  const uint steps=triangles_of_colour1()+vertices();
  uint step=0;
  
  progress_start(100,"Colouring");
  
  // Colour any triangles which need colouring (ie just sea)
  ByteRGBA colour_ocean(parameters.colour_ocean);
  ByteRGBA colour_river(parameters.colour_river);
  if (parameters.oceans_and_rivers_emissive>0.0f)
    {
      colour_ocean.a=0;
      colour_river.a=0;
    }

  for (uint i=triangles_of_colour0();i!=triangles();i++)
    {
      step++;
      progress_step((100*step)/steps);
      
      vertex(triangle(i).vertex(0)).colour(1,colour_ocean);
      vertex(triangle(i).vertex(1)).colour(1,colour_ocean);
      vertex(triangle(i).vertex(2)).colour(1,colour_ocean);
      
      // For debugging, set the colour0 of those triangles to red
      vertex(triangle(i).vertex(0)).colour(0,ByteRGBA(255,0,0,255));
      vertex(triangle(i).vertex(1)).colour(0,ByteRGBA(255,0,0,255));
      vertex(triangle(i).vertex(2)).colour(0,ByteRGBA(255,0,0,255));
    }
  
  const float treeline=0.25;
  const float beachline=0.01;
  
  // Colour all vertices
  for (uint i=0;i<vertices();i++)
    {
      step++;
      progress_step((100*step)/steps);      
      
      bool is_river=(river_vertices.find(i)!=river_vertices.end());
      
      float average_slope=1.0-(geometry().up(vertex(i).position())%vertex(i).normal());
      
      const float normalised_height=vertex_height(i)/max_height;
      
      float snowline_here=
	parameters.snowline_equator
	+fabs(geometry().normalised_latitude(vertex(i).position()))*(parameters.snowline_pole-parameters.snowline_equator)
	+parameters.snowline_slope_effect*average_slope
	-(is_river ? parameters.snowline_glacier_effect : 0.0);
      
      if (snowline_here>0.0)
	snowline_here=pow(snowline_here,parameters.snowline_power_law);
      
      if (normalised_height>snowline_here)
	{
	  vertex(i).colour(0,parameters.colour_snow);
	}
      else if (is_river)
	{
	  vertex(i).colour(0,parameters.colour_river);
	}
      else if (normalised_height<beachline)
	{
	  vertex(i).colour(0,parameters.colour_shoreline);
	}
      else if (normalised_height<treeline)
	{
	  const float blend=normalised_height/treeline;
	  vertex(i).colour(0,blend*parameters.colour_high+(1.0-blend)*parameters.colour_low);
	}
      else
	{
	  vertex(i).colour(0,parameters.colour_high);
	}
    }
  
  progress_complete("Colouring completed");
}

void TriangleMeshTerrain::do_terrain(const ParametersTerrain& parameters)
{
  do_noise(parameters);
  do_sea_level(parameters);
  do_power_law(parameters);
  do_rivers(parameters);
  compute_vertex_normals();
  do_colours(parameters);
  set_emissive(parameters.oceans_and_rivers_emissive);
}

void TriangleMeshTerrain::write_blender(std::ofstream& out,const ParametersSave&,const ParametersTerrain&,const std::string& mesh_name) const
{
  TriangleMesh::write_blender(out,mesh_name+".terrain",0);
}

namespace
{
  template <typename T> const T lerp(float l,const T& v0,const T& v1)
  {
    return (1.0f-l)*v0+l*v1;
  }

  FloatRGBA fn(const XYZ& v)
  {
    const XYZ n(v.normalised());
    return FloatRGBA(0.5f+0.5f*n.x,0.5f+0.5f*n.y,0.5f+0.5f*n.z,0.0f);
  }

  class ScanConvertHelper : public ScanConvertBackend
  {
  public:
    ScanConvertHelper
    (
     Raster<ByteRGBA>& image,
     Raster<ushort>* dem,
     Raster<ByteRGBA>* normalmap,
     const boost::array<FloatRGBA,3>& vertex_colours,
     const boost::array<float,3>& vertex_heights,
     const boost::array<XYZ,3>& vertex_normals
     )
      :ScanConvertBackend(image.width(),image.height())
       ,_image(image)
       ,_dem(dem)
       ,_normalmap(normalmap)
       ,_vertex_colours(vertex_colours)
       ,_vertex_heights(vertex_heights)
       ,_vertex_normals(vertex_normals)
    {
      if (_dem) assert(_image.width()==_dem->width() && _image.height()==_dem->height());
      if (_normalmap) assert(_image.width()==_normalmap->width() && _image.height()==_normalmap->height());
    }
    virtual ~ScanConvertHelper()
    {}
    
    virtual void scan_convert_backend(uint y,const ScanEdge& edge0,const ScanEdge& edge1) const
    {
      const FloatRGBA c0=lerp(edge0.lambda,_vertex_colours[edge0.vertex0],_vertex_colours[edge0.vertex1]);
      const FloatRGBA c1=lerp(edge1.lambda,_vertex_colours[edge1.vertex0],_vertex_colours[edge1.vertex1]);
      _image.scan(y,edge0.x,c0,edge1.x,c1);

      if (_dem)
	{
	  const float h0=lerp(edge0.lambda,_vertex_heights[edge0.vertex0],_vertex_heights[edge0.vertex1]);
	  const float h1=lerp(edge1.lambda,_vertex_heights[edge1.vertex0],_vertex_heights[edge1.vertex1]);
	  _dem->scan(y,edge0.x,h0,edge1.x,h1); 
	}
      if (_normalmap)
	{
	  const XYZ n0(lerp(edge0.lambda,_vertex_normals[edge0.vertex0],_vertex_normals[edge0.vertex1]).normalised());
	  const XYZ n1(lerp(edge1.lambda,_vertex_normals[edge1.vertex0],_vertex_normals[edge1.vertex1]).normalised());
	  _normalmap->scan<XYZ>(y,edge0.x,n0,edge1.x,n1,fn);
	}
    }

    virtual void subdivide(const boost::array<XYZ,3>& v,const XYZ& m,const ScanConverter& scan_converter) const
    {
      // Subdivision pattern (into 7) avoids creating any mid-points in edges shared with other triangles.
      const boost::array<XYZ,3> vm=
	{{
	  (v[1]+v[2]+m)/3.0f,
	  (v[0]+v[2]+m)/3.0f,
	  (v[0]+v[1]+m)/3.0f
	}};

      //! \todo This isn't right (for correct value would need to compute barycentric coordinates of m), but it will should only affect one facet at the pole.  
      const boost::array<FloatRGBA,3> cm=
	{{
	  0.5f*(_vertex_colours[1]+_vertex_colours[2]),
	  0.5f*(_vertex_colours[0]+_vertex_colours[2]),
	  0.5f*(_vertex_colours[0]+_vertex_colours[1])
	}};
      const boost::array<float,3> hm=
	{{
	  0.5f*(_vertex_heights[1]+_vertex_heights[2]),
	  0.5f*(_vertex_heights[0]+_vertex_heights[2]),
	  0.5f*(_vertex_heights[0]+_vertex_heights[1])
	}};
      const boost::array<XYZ,3> nm=
	{{
	  (_vertex_normals[1]+_vertex_normals[2]).normalised(),
	  (_vertex_normals[0]+_vertex_normals[2]).normalised(),
	  (_vertex_normals[0]+_vertex_normals[1]).normalised()
	}};

      {
	const boost::array<XYZ,3> p={{v[0],v[1],vm[2]}};
	const boost::array<FloatRGBA,3> c={{_vertex_colours[0],_vertex_colours[1],cm[2]}};
	const boost::array<float,3> h={{_vertex_heights[0],_vertex_heights[1],hm[2]}};
	const boost::array<XYZ,3> n={{_vertex_normals[0],_vertex_normals[1],nm[2]}};
	scan_converter.scan_convert(p,ScanConvertHelper(_image,_dem,_normalmap,c,h,n));
      }

      {
	const boost::array<XYZ,3> p={{v[1],v[2],vm[0]}};
	const boost::array<FloatRGBA,3> c={{_vertex_colours[1],_vertex_colours[2],cm[0]}};
	const boost::array<float,3> h={{_vertex_heights[1],_vertex_heights[2],hm[0]}};
	const boost::array<XYZ,3> n={{_vertex_normals[1],_vertex_normals[2],nm[0]}};
	scan_converter.scan_convert(p,ScanConvertHelper(_image,_dem,_normalmap,c,h,n));
      }

      {
	const boost::array<XYZ,3> p={{v[2],v[0],vm[1]}};
	const boost::array<FloatRGBA,3> c={{_vertex_colours[2],_vertex_colours[0],cm[1]}};
	const boost::array<float,3> h={{_vertex_heights[2],_vertex_heights[0],hm[1]}};
	const boost::array<XYZ,3> n={{_vertex_normals[2],_vertex_normals[0],nm[1]}};
	scan_converter.scan_convert(p,ScanConvertHelper(_image,_dem,_normalmap,c,h,n));
      }

      {
	const boost::array<XYZ,3> p={{v[0],vm[2],vm[1]}};
	const boost::array<FloatRGBA,3> c={{_vertex_colours[0],cm[2],cm[1]}};
	const boost::array<float,3> h={{_vertex_heights[0],hm[2],hm[1]}};
	const boost::array<XYZ,3> n={{_vertex_normals[0],nm[2],nm[1]}};
	scan_converter.scan_convert(p,ScanConvertHelper(_image,_dem,_normalmap,c,h,n));
      }

      {
	const boost::array<XYZ,3> p={{v[1],vm[0],vm[2]}};
	const boost::array<FloatRGBA,3> c={{_vertex_colours[1],cm[0],cm[2]}};
	const boost::array<float,3> h={{_vertex_heights[1],hm[0],hm[2]}};
	const boost::array<XYZ,3> n={{_vertex_normals[1],nm[0],nm[2]}};
	scan_converter.scan_convert(p,ScanConvertHelper(_image,_dem,_normalmap,c,h,n));
      }

      {
	const boost::array<XYZ,3> p={{v[2],vm[1],vm[0]}};
	const boost::array<FloatRGBA,3> c={{_vertex_colours[2],cm[1],cm[0]}};
	const boost::array<float,3> h={{_vertex_heights[2],hm[1],hm[0]}};
	const boost::array<XYZ,3> n={{_vertex_normals[2],nm[1],nm[0]}};
	scan_converter.scan_convert(p,ScanConvertHelper(_image,_dem,_normalmap,c,h,n));
      }

      {
	scan_converter.scan_convert(vm,ScanConvertHelper(_image,_dem,_normalmap,cm,hm,nm));
      }
      
    }
    
  private:
    Raster<ByteRGBA>& _image;
    Raster<ushort>* _dem;
    Raster<ByteRGBA>* _normalmap;
    const boost::array<FloatRGBA,3>& _vertex_colours;
    const boost::array<float,3>& _vertex_heights;
    const boost::array<XYZ,3>& _vertex_normals;
  };
}

void TriangleMeshTerrain::render_texture
(
 Raster<ByteRGBA>& image,
 Raster<ushort>* dem,
 Raster<ByteRGBA>* normal_map,
 bool shading,
 float ambient,
 const XYZ& illumination
 ) const
{
  progress_start(100,"Generating textures");

  for (uint i=0;i<triangles();i++)
    {
      const Triangle& t=triangle(i);
      const boost::array<const Vertex*,3> vertices
	={{
	  &vertex(t.vertex(0)),
	  &vertex(t.vertex(1)),
	  &vertex(t.vertex(2)),
	}};

      const boost::array<XYZ,3> vertex_positions
	={{
	  vertices[0]->position(),
	  vertices[1]->position(),
	  vertices[2]->position()
	}};

      const uint which_colour=(i<triangles_of_colour0() ? 0 : 1);
      const boost::array<FloatRGBA,3> vertex_colours
	={{
	  FloatRGBA(vertices[0]->colour(which_colour))*(shading ? ambient+(1.0f-ambient)*std::max(0.0f,vertices[0]->normal()%illumination) : 1.0f),
	  FloatRGBA(vertices[1]->colour(which_colour))*(shading ? ambient+(1.0f-ambient)*std::max(0.0f,vertices[1]->normal()%illumination) : 1.0f),
	  FloatRGBA(vertices[2]->colour(which_colour))*(shading ? ambient+(1.0f-ambient)*std::max(0.0f,vertices[2]->normal()%illumination) : 1.0f)
	}};
      const boost::array<float,3> vertex_heights
	={{
	  std::max(0.0f,std::min(65535.0f,65535.0f*geometry().height(vertices[0]->position()))),
	  std::max(0.0f,std::min(65535.0f,65535.0f*geometry().height(vertices[1]->position()))),
	  std::max(0.0f,std::min(65535.0f,65535.0f*geometry().height(vertices[2]->position())))
	}};
      const boost::array<XYZ,3> vertex_normals
	={{
	  vertices[0]->normal(),
	  vertices[1]->normal(),
	  vertices[2]->normal()
	}};

      ScanConvertHelper backend(image,dem,normal_map,vertex_colours,vertex_heights,vertex_normals);

      geometry().scan_convert
	(
	 vertex_positions,
	 backend
	 );

      progress_step((100*i)/(triangles()-1));
    }

  progress_complete("Texture generation completed");
}

TriangleMeshTerrainPlanet::TriangleMeshTerrainPlanet(const ParametersTerrain& parameters,Progress* progress)
  :TriangleMesh(progress)
  ,TriangleMeshTerrain(progress)
  ,TriangleMeshSubdividedIcosahedron(1.0+parameters.variation.z*parameters.base_height,parameters.subdivisions,parameters.subdivisions_unperturbed,parameters.seed,parameters.variation,progress)
{
  do_terrain(parameters);
}

void TriangleMeshTerrainPlanet::write_povray(std::ofstream& out,const ParametersSave& param_save,const ParametersTerrain& parameters_terrain) const
{
  if (param_save.pov_sea_object)
    {
      out 
	<< "sphere {<0.0,0.0,0.0>,1.0 pigment{rgb "
	<< parameters_terrain.colour_ocean.format_pov_rgb()
	<< "} finish {ambient " 
	<< emissive() 
	<< " diffuse " 
	<< 1.0f-emissive() 
	<< "}}\n";
    }
  
  if (param_save.pov_atmosphere)
    {
      out
	<< "sphere {<0.0,0.0,0.0>,1.025 hollow texture {pigment {color rgbf 1}} interior{media{scattering{1,color rgb <1.0,1.0,1.0> extinction 1}}}}\n"
	<< "sphere {<0.0,0.0,0.0>,1.05  hollow texture {pigment {color rgbf 1}} interior{media{scattering{1,color rgb <0.0,0.0,1.0> extinction 1}}}}\n";
    }
  
  TriangleMesh::write_povray(out,param_save.pov_sea_object,false,false); // Don't double illuminate.  Don't no-shadow.
}

TriangleMeshTerrainFlat::TriangleMeshTerrainFlat(const ParametersTerrain& parameters,Progress* progress)
  :TriangleMesh(progress)
  ,TriangleMeshTerrain(progress)
  ,TriangleMeshFlat(parameters.object_type,parameters.variation.z*parameters.base_height,parameters.seed,progress)
{
  subdivide(parameters.subdivisions,parameters.subdivisions_unperturbed,parameters.variation);

  do_terrain(parameters);
}

void TriangleMeshTerrainFlat::write_povray(std::ofstream& out,const ParametersSave& param_save,const ParametersTerrain& parameters_terrain) const
{
  if (param_save.pov_sea_object)
    {
      out
	<< "plane {<0.0,1.0,0.0>,0.0 pigment{rgb "
	<< parameters_terrain.colour_ocean.format_pov_rgb()
	<< "} finish {ambient " 
	<< emissive() 
	<< " diffuse " 
	<< 1.0f-emissive() 
	<< "}}\n";
    }
  
  if (param_save.pov_atmosphere)
    {
      out
	<< "plane {<0.0,1.0,0.0>,0.05 hollow texture {pigment {color rgbf 1}} interior{media{scattering{1,color rgb <1.0,1.0,1.0> extinction 1}}}}\n"
	<< "plane {<0.0,1.0,0.0>,0.1  hollow texture {pigment {color rgbf 1}} interior{media{scattering{1,color rgb <0.0,0.0,1.0> extinction 1}}}}\n";
    }
  
  TriangleMesh::write_povray(out,param_save.pov_sea_object,false,false); // Don't double illuminate.  Don't no-shadow.
}
