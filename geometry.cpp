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

#include "geometry.h"

/*! Common scan-converter code
 */
void Geometry::scan_convert_common
(
 const boost::array<XYZ,3>& v,
 const ScanConvertBackend& backend
 )
{
  // Sort vertices by increasing image y co-ordinate 
  boost::array<uint,3> sort={{0,1,2}};
  if (v[sort[0]].y>v[sort[1]].y) exchange(sort[0],sort[1]);
  if (v[sort[1]].y>v[sort[2]].y) exchange(sort[1],sort[2]);
  if (v[sort[0]].y>v[sort[1]].y) exchange(sort[0],sort[1]);

  // deltas
  const float x02=v[sort[2]].x-v[sort[0]].x;
  const float y02=v[sort[2]].y-v[sort[0]].y;
  const float x01=v[sort[1]].x-v[sort[0]].x;
  const float y01=v[sort[1]].y-v[sort[0]].y;
  const float x12=v[sort[2]].x-v[sort[1]].x;
  const float y12=v[sort[2]].y-v[sort[1]].y;

  // Use "complete" make_optional to avoid a maybe-used-uninitialized warning later.
  boost::optional<float> ky02=boost::make_optional(false,0.0f);
  boost::optional<float> ky01=boost::make_optional(false,0.0f);
  boost::optional<float> ky12=boost::make_optional(false,0.0f);
  
  if (y02==0.0f) return;
  
  ky02=1.0f/y02;
  if (y01!=0.0f) ky01=1.0f/y01;
  if (y12!=0.0f) ky12=1.0f/y12;
  
  // y range in image co-ordinates
  const int map_height=backend.height();
  const int iy_min=static_cast<int>(std::max(0.0f,ceilf(v[sort[0]].y-0.5f)));
  const int iy_mid=static_cast<int>(floorf(v[sort[1]].y-0.5f));
  const int iy_max=static_cast<int>(std::min(map_height-0.5f,floorf(v[sort[2]].y-0.5f)));
  
  if (ky01)
    for (int iy=iy_min;iy<=iy_mid;iy++)
      {
	const float yp02=(iy+0.5f-v[sort[0]].y)*ky02.get();
	const ScanEdge edge0(v[sort[0]].x+yp02*x02,sort[0],sort[2],yp02);
	
	const float yp01=(iy+0.5f-v[sort[0]].y)*ky01.get();
	const ScanEdge edge1(v[sort[0]].x+yp01*x01,sort[0],sort[1],yp01);
	if (edge0.x<=edge1.x)
	  backend.scan_convert_backend(iy,edge0,edge1);
	else
	  backend.scan_convert_backend(iy,edge1,edge0);
      }
  
  if (ky12)
    for (int iy=iy_mid+1;iy<=iy_max;iy++)
      {
	const float yp02=(iy+0.5f-v[sort[0]].y)*ky02.get();
	const ScanEdge edge0(v[sort[0]].x+yp02*x02,sort[0],sort[2],yp02);
	
	const float yp12=(iy+0.5f-v[sort[1]].y)*ky12.get();
	const ScanEdge edge1(v[sort[1]].x+yp12*x12,sort[1],sort[2],yp12);
	if (edge0.x<=edge1.x)
	  backend.scan_convert_backend(iy,edge0,edge1);
	else
	  backend.scan_convert_backend(iy,edge1,edge0);
      }
}

/*! Scan lines are through the centre of pixels at y=0.5.
  This function doesn't care about quantization in x; that's for the backend.
 */
void GeometryFlat::scan_convert
(
 const boost::array<XYZ,3>& v,
 const ScanConvertBackend& backend
 ) const
{
  const boost::array<XYZ,3> vp
    ={{
      XYZ(backend.width()*0.5f*(1.0f+v[0].x),backend.height()*0.5f*(1.0f-v[0].y),0.0f),
      XYZ(backend.width()*0.5f*(1.0f+v[1].x),backend.height()*0.5f*(1.0f-v[1].y),0.0f),
      XYZ(backend.width()*0.5f*(1.0f+v[2].x),backend.height()*0.5f*(1.0f-v[2].y),0.0f)
    }};

  scan_convert_common(vp,backend);
}

/*!
  The problem with spherical geometry is that spans can go off one side of the map and come back on the other.
*/
void GeometrySpherical::scan_convert
(
 const boost::array<XYZ,3>& v,
 const ScanConvertBackend& backend
 ) const
{
  const boost::array<XYZ,3> vn={{v[0].normalised(),v[1].normalised(),v[2].normalised()}};

  const bool coplanar=(fabsf((vn[0]*vn[1]).normalised()%vn[2]) < 1e-6f);
  if (coplanar) return;

  {
    const XYZ pole(0.0f,0.0f,1.0f);
    const float p01=pole%(v[0]*v[1]);
    const float p12=pole%(v[1]*v[2]);
    const float p20=pole%(v[2]*v[0]);
    const bool contains_pole=((p01>=0.0f && p12>=0.0f && p20>=0.0f) || (p01<=0.0f && p12<=0.0f && p20<=0.0f));
    if (contains_pole)
      {
	// Don't subdivide when furthest vertex is so close it won't be rendered
	const float mx=std::max(fabsf(vn[0].x),std::max(fabsf(vn[1].x),fabsf(vn[2].x)));
	const float my=std::max(fabsf(vn[0].y),std::max(fabsf(vn[1].y),fabsf(vn[2].y)));
	const float m=std::max(mx,my);
	if (m<0.25f/backend.height())
	  return;
	else
	  {
	    const XYZ which_pole(0.0f,0.0f,((v[0]+v[1]+v[2]).z>0.0f? 1.0f : -1.0f));
	    backend.subdivide(v,which_pole,*this);
	  }
      }
  }
  
  boost::array<XYZ,3> vp;
  for (uint i=0;i<3;i++)
    {
      vp[i].x=backend.width()*0.5f*(1.0f+M_1_PI*atan2f(vn[i].y,vn[i].x));  // atan2f returns [-pi to +pi] so vp[0] in [0,width] 
      if (i!=0)
	{
	  // Need to keep all the vertices in the same domain
	  if (vp[i].x-vp[0].x>0.5*backend.width()) vp[i].x-=backend.width();
	  else if (vp[i].x-vp[0].x<-0.5*backend.width()) vp[i].x+=backend.width();
	}
      vp[i].y=backend.height()*0.5f*(1.0f-asinf(vn[i].z)*M_2_PI);
      vp[i].z=0.0f;
    }
  for (float d=-1.0f;d<=1.0f;d+=1.0f)
    {
      // Easiest way to deal with triangles on the "date line" is to
      // render them with all possible placements and let the back end cull them.
      /*! \todo Might be better if span replication was done in backed rather than
	duplicating all the y-compute.
      */
      boost::array<XYZ,3> vpt;
      for (uint i=0;i<3;i++)
	{
	  vpt[i].x=vp[i].x+d*backend.width();
	  vpt[i].y=vp[i].y;
	  vpt[i].z=vp[i].z;
	}
      scan_convert_common(vpt,backend);
    }
}
