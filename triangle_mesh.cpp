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

#include "triangle_mesh.h"

#include <map>

TriangleMesh::TriangleMesh(Progress* progress)
  :_triangle_switch_colour(0)
   ,_emissive(0.0)
   ,_progress(progress)
{}

TriangleMesh::~TriangleMesh()
{}

void TriangleMesh::progress_start(uint steps,const std::string& info) const
{
  if (_progress) _progress->progress_start(steps,info);
}
void TriangleMesh::progress_stall(const std::string& reason) const
{
  if (_progress) _progress->progress_stall(reason);
}
void TriangleMesh::progress_step(uint step) const
{
  if (_progress) _progress->progress_step(step);
}
void TriangleMesh::progress_complete(const std::string& info) const
{
  if (_progress) _progress->progress_complete(info);
}

const XYZ TriangleMesh::triangle_normal(uint i) const
{
  const Triangle& t=triangle(i);
  const XYZ& v0=vertex(t.vertex(0)).position();
  const XYZ& v1=vertex(t.vertex(1)).position();
  const XYZ& v2=vertex(t.vertex(2)).position();

  const XYZ n((v1-v0)*(v2-v0));

  return n.normalised();
}

void TriangleMesh::compute_vertex_normals()
{
  const uint steps=triangles()+vertices();
  uint step=0;

  progress_start(100,"Compute normals");

  {
    std::vector<std::vector<uint> > vertices_to_triangles(vertices());
    for (uint i=0;i<triangles();i++)
      {
    step++;
    progress_step((100*step)/steps);

    const Triangle& t=triangle(i);
    vertices_to_triangles[t.vertex(0)].push_back(i);
    vertices_to_triangles[t.vertex(1)].push_back(i);
    vertices_to_triangles[t.vertex(2)].push_back(i);
      }

    for (uint i=0;i<vertices();i++)
      {
    step++;
    progress_step((100*step)/steps);

    XYZ n(0.0,0.0,0.0);
    for (uint j=0;j<vertices_to_triangles[i].size();j++)
      {
        n+=triangle_normal(vertices_to_triangles[i][j]);
      }
    n/=vertices_to_triangles[i].size();

    vertex(i).normal(n);
      }
  }

  progress_complete("Normals computed");
}

/*! level parameter is just for progress information
 */
void TriangleMesh::subdivide(const XYZ& variation,uint level,uint levels)
{
  const uint steps=vertices()+triangles();
  uint step=0;

  {
    std::ostringstream msg;
    msg
      << "Subdivision level "
      << 1+level  // Display 1...levels inclusive
      << " of "
      << levels;
    progress_start(100,msg.str());
  }

  {
    // Efficiently move aside current geometry, get ready for fresh.
    std::vector<Vertex> old_vertex;
    old_vertex.swap(_vertex);

    std::vector<Triangle> old_triangle;
    old_triangle.swap(_triangle);

    // Copy back all vertices and perturb
    for (uint v=0;v<old_vertex.size();v++)
      {
    step++;
    progress_step((100*step)/steps);

    _vertex.push_back(Vertex(geometry().perturb(old_vertex[v].position(),variation)));
      }

    // Build a map from edges to new midpoints
    typedef std::map<TriangleEdge,uint> EdgeMap;
    EdgeMap edge_map;

    // Create new vertices and new triangles
    for (uint t=0;t<old_triangle.size();t++)
      {
    step++;
    progress_step((100*step)/steps);

    // These are the existing vertices
    const uint i0=old_triangle[t].vertex(0);
    const uint i1=old_triangle[t].vertex(1);
    const uint i2=old_triangle[t].vertex(2);

    // These are the edges
    TriangleEdge e01(i0,i1);
    TriangleEdge e12(i1,i2);
    TriangleEdge e20(i2,i0);

    // Find each edge in the map
    // If any edges don't exist, create their new mid-point and remember it
    EdgeMap::const_iterator e01it=edge_map.find(e01);
    EdgeMap::const_iterator e12it=edge_map.find(e12);
    EdgeMap::const_iterator e20it=edge_map.find(e20);

    const bool e01needed=(e01it==edge_map.end());
    const bool e12needed=(e12it==edge_map.end());
    const bool e20needed=(e20it==edge_map.end());

    if (e01needed)
      {
        e01it=edge_map.insert(EdgeMap::value_type(e01,_vertex.size())).first;
        _vertex.push_back(Vertex(geometry().perturb(geometry().midpoint(vertex(i0).position(),vertex(i1).position()),variation)));
      }

    if (e12needed)
      {
        e12it=edge_map.insert(EdgeMap::value_type(e12,_vertex.size())).first;
        _vertex.push_back(Vertex(geometry().perturb(geometry().midpoint(vertex(i1).position(),vertex(i2).position()),variation)));
      }

    if (e20needed)
      {
        e20it=edge_map.insert(EdgeMap::value_type(e20,_vertex.size())).first;
        _vertex.push_back(Vertex(geometry().perturb(geometry().midpoint(vertex(i2).position(),vertex(i0).position()),variation)));
      }

    // Create the subdivided triangles

    _triangle.push_back(Triangle(i0,(*e01it).second,(*e20it).second));
    _triangle.push_back(Triangle((*e01it).second,i1,(*e12it).second));
    _triangle.push_back(Triangle((*e20it).second,(*e12it).second,i2));
    _triangle.push_back(Triangle((*e01it).second,(*e12it).second,(*e20it).second));
      }
  }

  progress_complete("Subdivision completed");
}

void TriangleMesh::subdivide(uint subdivisions,uint flat_subdivisions,const XYZ& variation)
{
  for (uint s=0;s<subdivisions;s++)
    {
      if (s<flat_subdivisions)
    subdivide(XYZ(0.0,0.0,0.0),s,subdivisions);
      else
    subdivide(variation/(1<<s),s,subdivisions);
    }
}

void TriangleMesh::write_povray(std::ofstream& out,bool exclude_alternate_colour,bool double_illuminate,bool no_shadow) const
{
  // \todo: No need to dump all vertices when not outputing all triangles.

  const uint triangles_to_output=(exclude_alternate_colour ? triangles_of_colour0() : triangles());

  // The number of steps is:
  //   vertices() co-ordinates
  // + vertices()+(exclude_alternate_colour ? 0 : vertices()) textures
  // + triangles_to_output triangles

  const uint steps=vertices()+vertices()+(exclude_alternate_colour ? 0 : vertices())+triangles_to_output;
  uint step=0;

  progress_start(100,"Writing mesh to POV-Ray file");

  // Use POV's mesh2 object

  out << "mesh2 {\n";

  // Output all the vertex co-ordinates
  out << "vertex_vectors {" << vertices() << ",\n";

  for (uint v=0;v<vertices();v++)
    {
      step++;
      progress_step((100*step)/steps);

      if (v!=0)
    out << ",";
      out << vertex(v).position().format_pov() << "\n";
    }
  out << "}\n";

  // Output the vertex colours, and handle emission
  // If exclude_alternate_colour is true, don't output the alternate colours
  out << "texture_list {" << vertices()+(exclude_alternate_colour ? 0 : vertices()) << "\n";

  for (uint c=0;c<(exclude_alternate_colour ? 1 : 2);c++)
    for (uint v=0;v<vertices();v++)
      {
    step++;
    progress_step((100*step)/steps);

    out << "texture{pigment{";
    const FloatRGBA colour(vertex(v).colour(c));
    if (colour.a==1.0f) out << "rgb " << colour.format_pov_rgb();
    else out << "rgbf " << colour.format_pov_rgbf();
    out << "}";

    if (emissive()!=0.0f && vertex(v).colour(c).a==0)
      {
        out << " finish{ambient " << emissive() << " diffuse " << 1.0f-emissive() << "}";
      }
    out << "}\n";
      }

  out << "}\n";

  out << "face_indices {" << triangles_to_output << ",\n";
  bool skip_initial_comma=true;
  for (uint t=0;t<triangles_to_output;t++)
    {
      step++;
      progress_step((100*step)/steps);

      if (skip_initial_comma)
    skip_initial_comma=false;
      else
    out << ",";

      out
    << "<"
    << triangle(t).vertex(0)
    << ","
    << triangle(t).vertex(1)
    << ","
    << triangle(t).vertex(2)
    << ">";

      out << "," << triangle(t).vertex(0)+(t<triangles_of_colour0() ? 0 : vertices());
      out << "," << triangle(t).vertex(1)+(t<triangles_of_colour0() ? 0 : vertices());
      out << "," << triangle(t).vertex(2)+(t<triangles_of_colour0() ? 0 : vertices());
      out << "\n";
    }
  out << "}\n";
  if (double_illuminate) out << "double_illuminate\n";
  if (no_shadow) out << "no_shadow\n";
  out << "}\n";

  progress_complete("Wrote mesh to POV-Ray file");
}

/*! If faux_alpha is null, output per-vertex alpha.
  If a colour is specified, use the vertex alpha to blend with it.
 */
void TriangleMesh::write_blender(std::ofstream& out,const std::string& mesh_name,const FloatRGBA* faux_alpha) const
  {
    std::unique_ptr<ByteRGBA> byte_faux_alpha;
    if (faux_alpha) byte_faux_alpha=std::unique_ptr<ByteRGBA>(new ByteRGBA(*faux_alpha));

    const uint steps=vertices()+triangles();
    uint step=0;

    {
      std::ostringstream msg;
      msg << "Writing mesh " << mesh_name << " to Blender script file";
      progress_start(100,msg.str());
    }

    out <<
        "mat0 = bpy.data.materials.new(\"fracplanet0\")\n"
        "mat0.diffuse_color = (0.0, 1.0, 0.0)\n"
        "mat0.use_vertex_color_paint = True\n"
        "bpy.ops.object.material_slot_add()\n"
        "the_mesh_obj.material_slots[-1].material = mat0\n"
        "mat1 = bpy.data.materials.new(\"fracplanet1\")\n"
        "mat1.diffuse_color = (0.0, 0.0, 1.0)\n"
        "mat1.use_vertex_color_paint = True\n"
        "bpy.ops.object.material_slot_add()\n"
        "the_mesh_obj.material_slots[-1].material = mat1\n";

    for (uint v=0;v<vertices();v++)
      {
        step++;
        progress_step((100*step)/steps);
        out << "v(" << vertex(v).position().format_blender() << ")\n";
      }

    out << "\n";

    for (uint t=0;t<triangles();t++)
      {
        step++;
        progress_step((100*step)/steps);
        const uint v0=triangle(t).vertex(0);
        const uint v1=triangle(t).vertex(1);
        const uint v2=triangle(t).vertex(2);
        const uint c=(t<triangles_of_colour0() ? 0 : 1);
        out
      << "f("
      << c << ", "
      << v0 << ", "
      << v1 << ", "
      << v2 << ", "
      << "(" << blender_alpha_workround(byte_faux_alpha.get(),vertex(v0).colour(c)).format_comma() << "), "
      << "(" << blender_alpha_workround(byte_faux_alpha.get(),vertex(v1).colour(c)).format_comma() << "), "
      << "(" << blender_alpha_workround(byte_faux_alpha.get(),vertex(v2).colour(c)).format_comma() << ")"
      << ")\n";
      }

    std::ostringstream msg;
    msg << "Wrote mesh " << mesh_name << " to Blender script file";
    progress_complete(msg.str());
  } /*TriangleMesh::write_blender*/

ByteRGBA TriangleMesh::blender_alpha_workround(const ByteRGBA* f,const ByteRGBA& c)
{
  if (f)
    {
      const uint ia=static_cast<uint>(c.a);
      return ByteRGBA
    (
     (ia*c.r+(255-ia)*f->r)/255,
     (ia*c.g+(255-ia)*f->g)/255,
     (ia*c.b+(255-ia)*f->b)/255,
     255
     );
    }
  else
    return c;
}

TriangleMeshFlat::TriangleMeshFlat(ParametersObject::ObjectType obj,float z,uint seed,Progress* progress)
:TriangleMesh(progress)
 ,_geometry(seed)
{
  switch(obj)
    {
    case ParametersObject::ObjectTypeFlatTriangle:
      for (uint i=0;i<3;i++)
    {
      add_vertex(Vertex(XYZ(cos(i*2.0*M_PI/3.0),sin(i*2.0*M_PI/3.0),z)));
    }
      add_triangle(Triangle(0,1,2));
      break;

    case ParametersObject::ObjectTypeFlatSquare:
      add_vertex(Vertex(XYZ( 0.0, 0.0,z)));
      add_vertex(Vertex(XYZ( 1.0, 1.0,z)));
      add_vertex(Vertex(XYZ(-1.0, 1.0,z)));
      add_vertex(Vertex(XYZ(-1.0,-1.0,z)));
      add_vertex(Vertex(XYZ( 1.0,-1.0,z)));

      for (uint i=0;i<4;i++)
    {
      add_triangle(Triangle(0,1+i,1+(i+1)%4));
    }

      break;

    case ParametersObject::ObjectTypeFlatHexagon:
    default:
      add_vertex(Vertex(XYZ(0.0,0.0,z)));
      for (uint i=0;i<6;i++)
    {
      add_vertex(Vertex(XYZ(cos(i*M_PI/3.0),sin(i*M_PI/3.0),z)));
    }

      for (uint i=0;i<6;i++)
    {
      add_triangle(Triangle(0,1+i,1+(i+1)%6));
    }
      break;
    }
}

TriangleMeshIcosahedron::TriangleMeshIcosahedron(float radius,uint seed,Progress* progress)
:TriangleMesh(progress)
 ,_geometry(seed)
{
  const float x=0.525731112119133606;
  const float z=0.850650808352039932;

  const float vdata[12][3]=
    {
      { -x,0.0,  z},
      {  x,0.0,  z},
      { -x,0.0, -z},
      {  x,0.0, -z},
      {0.0,  z,  x},
      {0.0,  z, -x},
      {0.0, -z,  x},
      {0.0, -z, -x},
      {  z,  x,0.0},
      { -z,  x,0.0},
      {  z, -x,0.0},
      { -z, -x,0.0}
    };

  for (uint v=0;v<12;v++)
    add_vertex(Vertex(radius*XYZ(vdata[v][0],vdata[v][1],vdata[v][2]).normalised()));

  uint tindices[20][3]=
    {
      { 0, 4, 1},
      { 0, 9, 4},
      { 9, 5, 4},
      { 4, 5, 8},
      { 4, 8, 1},
      { 8,10, 1},
      { 8, 3,10},
      { 5, 3, 8},
      { 5, 2, 3},
      { 2, 7, 3},
      { 7,10, 3},
      { 7, 6,10},
      { 7,11, 6},
      {11, 0, 6},
      { 0, 1, 6},
      { 6, 1,10},
      { 9, 0,11},
      { 9,11, 2},
      { 9, 2, 5},
      { 7, 2,11}
    };

  for (uint t=0;t<20;t++)
    add_triangle(Triangle(tindices[t][2],tindices[t][1],tindices[t][0]));
}

TriangleMeshSubdividedIcosahedron::TriangleMeshSubdividedIcosahedron(float radius,uint subdivisions,uint flat_subdivisions,uint seed,const XYZ& variation,Progress* progress)
  :TriangleMesh(progress)
  ,TriangleMeshIcosahedron(radius,seed,progress)
{
  subdivide(subdivisions,flat_subdivisions,variation);
}

