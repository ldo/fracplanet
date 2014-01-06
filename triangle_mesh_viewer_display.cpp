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

#include "precompiled.h"

#include "triangle_mesh_viewer_display.h"

#include "matrix33.h"
#include "triangle_mesh_viewer.h"

#include <GL/glu.h>

TriangleMeshViewerDisplay::TriangleMeshViewerDisplay(TriangleMeshViewer* parent,const QGLFormat& format,const ParametersRender* param,const std::vector<const TriangleMesh*>& m,bool verbose)
  :QGLWidget(format,parent)
  ,_notify(*parent)
  ,_verbose(verbose)
  ,mesh(m)
  ,parameters(param)
  ,gl_display_list_index(0)
  ,frame_number(0)
  ,width(0)
  ,height(0)
  ,frame_time()
  ,camera_position(3.0f,0.0f,0.0f)
  ,camera_lookat(0.0f,0.0f,0.0f)
  ,camera_up(0.0f,0.0f,1.0f)
  ,object_tilt(30.0f*M_PI/180.0f)
  ,object_rotation(0.0f)
{
  assert(isValid());
  
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

  frame_time.start();
  frame_time_reported.start();
}

TriangleMeshViewerDisplay::~TriangleMeshViewerDisplay()
{
  makeCurrent();
  if (gl_display_list_index)
    glDeleteLists(gl_display_list_index,1);
}

QSize TriangleMeshViewerDisplay::minimumSizeHint() const
{
  return QSize(64,64);
}

QSize TriangleMeshViewerDisplay::sizeHint() const
{
  return QSize(512,512);
}

void TriangleMeshViewerDisplay::set_mesh(const std::vector<const TriangleMesh*>& m)
{
  mesh=m;

  // If there is a display list allocated for the previous mesh, delete it.
  if (gl_display_list_index!=0)
    {
      glDeleteLists(gl_display_list_index,1);
      gl_display_list_index=0;
    }  
}

const FloatRGBA TriangleMeshViewerDisplay::background_colour() const
{
  if (mesh.empty()) return FloatRGBA(0.0f,0.0f,0.0f,1.0f);

  const XYZ relative_camera_position 
    =Matrix33RotateAboutZ(-object_rotation)*Matrix33RotateAboutX(-object_tilt)*camera_position;

  const float h = mesh[0]->geometry().height(relative_camera_position);
  if (h<=0.0f) return parameters->background_colour_low;
  else if (h>=1.0f) return parameters->background_colour_high;
  else return parameters->background_colour_low+h*(parameters->background_colour_high-parameters->background_colour_low);
}

void TriangleMeshViewerDisplay::check_for_gl_errors(const char* where) const
{
  GLenum error;
  while ((error=glGetError())!=GL_NO_ERROR)
    {
      std::ostringstream msg;
      msg << "GL error in " << where << " (frame " << frame_number << ") : ";
      switch (error)
	{
	case GL_INVALID_ENUM:
	  msg << "GL_INVALID_ENUM";
	  break;
	case GL_INVALID_VALUE:
	  msg << "GL_INVALID_VALUE";
	  break;
	case GL_INVALID_OPERATION:
	  msg << "GL_INVALID_OPERATION";
	  break;
	case GL_STACK_OVERFLOW:
	  msg << "GL_STACK_OVERFLOW";
	  break;
	case GL_STACK_UNDERFLOW:
	  msg << "GL_STACK_UNDERFLOW";
	  break;
	case GL_OUT_OF_MEMORY:
	  msg << "GL_OUT_OF_MEMORY";
	  break;
	}
      std::cerr << msg.str() << std::endl;
    }
}

void TriangleMeshViewerDisplay::paintGL()
{
  assert(isValid());

  const FloatRGBA bg=background_colour();
  glClearColor(bg.r,bg.g,bg.b,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const float a=parameters->ambient;

  GLfloat global_ambient[]={a,a,a,1.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,global_ambient);

  GLfloat light_diffuse[]={1.0-a,1.0-a,1.0-a,1.0};
  glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(
	    camera_position.x,camera_position.y,camera_position.z,
	    camera_lookat.x  ,camera_lookat.y  ,camera_lookat.z,
	    camera_up.x      ,camera_up.y      ,camera_up.z
	    );

  const XYZ light_direction(parameters->illumination_direction());
  GLfloat light_position[]=
    {
      light_direction.x,
      light_direction.y,
      light_direction.z,
      0.0f     // w=0 implies directional light
    };

  glLightfv(GL_LIGHT0,GL_POSITION,light_position);

  glRotatef((180.0/M_PI)*object_tilt,1.0,0.0,0.0);
  glRotatef((180.0/M_PI)*object_rotation,0.0,0.0,1.0);

  glPolygonMode(GL_FRONT_AND_BACK,(parameters->wireframe ? GL_LINE : GL_FILL));

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  
  if (parameters->display_list && gl_display_list_index!=0)
    {
      glCallList(gl_display_list_index);
    }
  else
    {
      bool building_display_list=(parameters->display_list && gl_display_list_index==0);

      if (building_display_list)
	{
	  gl_display_list_index=glGenLists(1);
	  assert(gl_display_list_index!=0);
	  glNewList(gl_display_list_index,GL_COMPILE_AND_EXECUTE);
	  if (_verbose) std::cerr << "Building display list...";
	}

      GLfloat default_material_white[3]={1.0f,1.0f,1.0f};
      GLfloat default_material_black[3]={0.0f,0.0f,0.0f};
      glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,default_material_white);
      glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,default_material_black);

      for (uint m=0;m<mesh.size();m++)
	{
	  const TriangleMesh*const it=mesh[m];
	  if (it==0) continue;

	  // Meshes after the first are rendered twice: first the backfacing polys then the front facing.
	  // This solves the problem of either clouds disappearing when we're under them (with backface culling)
	  // or weird stuff around the periphery when culling is on.
	  // It's quite an expensive solution!  
	  const uint passes=(m==0 ? 1 : 2);
	  for (uint pass=0;pass<passes;pass++)
	    {
	      if (passes==2 && pass==0)
		{
		  glCullFace(GL_FRONT);
		}
	      else
		{
		  glCullFace(GL_BACK);
		}

	      if (it->emissive()==0.0f)
		{
		  if (m) // Switch blending on for non-emissive meshes after the first
		    {
		      glEnable(GL_BLEND);
		      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		    }
		  else
		    {
		      glDisable(GL_BLEND);
		    }
		  
		  // Use "Color Material" mode 'cos everything is the same material.... just change the colour
		  glEnable(GL_COLOR_MATERIAL);
		  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
		  
		  // Point GL at arrays of data
		  glVertexPointer(3,GL_FLOAT,sizeof(Vertex),&(it->vertex(0).position().x));
		  glNormalPointer(GL_FLOAT,sizeof(Vertex),&(it->vertex(0).normal().x));
		  
		  // For a second mesh, use alpha (actually could use it for the first mesh but maybe it's more efficient not to).
		  glColorPointer((m==0 ? 3 : 4),GL_UNSIGNED_BYTE,sizeof(Vertex),&(it->vertex(0).colour(0).r));

		  // Builds on some platforms (ie Ubuntu) seem to get in a mess if you render >1k primitives
		  // (3k vertices).  NB This is a problem in the client; not the xserver.
		  // Debian (Sarge or Etch) has no problems with unlimited batches.
		  // Note it's simply the batch size; there doesn't seem to be any problem with the 10Ks of vertices.
		  // Since the limited batch size doesn't seem to hurt working implementations we just use it everywhere.
		  const uint batch_size=1000;

		  // Draw the colour-zero triangles
		  for (uint t=0;t<it->triangles_of_colour0();t+=batch_size)
		    {
		      glDrawRangeElements
			(
			 GL_TRIANGLES,
			 0,
			 it->vertices()-1,
			 3*std::min(batch_size,static_cast<uint>(it->triangles_of_colour0()-t)),
			 GL_UNSIGNED_INT,
			 &(it->triangle(t).vertex(0))
			 );
		      if (_verbose && building_display_list)
			{
			  std::cerr << ".";
			}
		    }
		  
		  // Switch to alternate colour and draw the colour-one triangles
		  glColorPointer(3,GL_UNSIGNED_BYTE,sizeof(Vertex),&(it->vertex(0).colour(1).r));

		  for (uint t=it->triangles_of_colour0();t<it->triangles();t+=batch_size)
		    {
		      glDrawRangeElements
			(
			 GL_TRIANGLES,
			 0,
			 it->vertices()-1,
			 3*std::min(batch_size,static_cast<uint>(it->triangles()-t)),
			 GL_UNSIGNED_INT,
			 &(it->triangle(t).vertex(0))
			 );
		      if (_verbose && building_display_list)
			{
			  std::cerr << ".";
			}
		    }
		  
		  glDisable(GL_COLOR_MATERIAL);
		}
	      else // implies mesh[m]->emissive()>0.0
		{
		  // We abuse alpha for emission, so no blending
		  glDisable(GL_BLEND);
		  
		  // If there could be emissive vertices, we need to do things the hard way
		  // using immediate mode.  Maybe the display list capture will help.
		  
		  const float k=1.0f/255.0f;
		  const float em=k*(     it->emissive());
		  const float ad=k*(1.0f-it->emissive());
		  
		  glBegin(GL_TRIANGLES);
		  for (unsigned int t=0;t<it->triangles();t++)
		    {
		      if (_verbose && building_display_list && (t&0x3ff) == 0)
			{
			  std::cerr << ".";
			}

		      const uint c=(t<it->triangles_of_colour0() ? 0 : 1);		      
		      for (uint i=0;i<3;i++)
			{
			  const uint vn=it->triangle(t).vertex(i);
			  const Vertex& v=it->vertex(vn);
			  
			  GLfloat c_ad[3];
			  GLfloat c_em[3];
			  if (v.colour(c).a==0)  // Zero alpha used to imply emissive vertex colour
			    {
			      c_ad[0]=v.colour(c).r*ad;
			      c_ad[1]=v.colour(c).g*ad;
			      c_ad[2]=v.colour(c).b*ad;
			      c_em[0]=v.colour(c).r*em;
			      c_em[1]=v.colour(c).g*em;
			      c_em[2]=v.colour(c).b*em;
			    }
			  else
			    {
			      c_ad[0]=v.colour(c).r*k;
			      c_ad[1]=v.colour(c).g*k;
			      c_ad[2]=v.colour(c).b*k;
			      c_em[0]=0.0f;
			      c_em[1]=0.0f;
			      c_em[2]=0.0f;
			    }
			  
			  glNormal3fv(&(v.normal().x));
			  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,c_ad);
			  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,c_em);
			  glVertex3fv(&(v.position().x));
			}
		    }
		  glEnd();
		}
	    }
	}
      
      if (building_display_list)
	{
	  glEndList();
	  if (_verbose)
	    {
	      std::cerr << "\n...built display list\n";
	    }
	}
    }

  if (_verbose)
    check_for_gl_errors(__PRETTY_FUNCTION__);

  // Get time taken since last frame
  const uint dt=frame_time.restart();

  // Save it in the queue
  frame_times.push_back(dt);

  // Keep last 30 frame times
  while (frame_times.size()>30) frame_times.pop_front();

  // Only update frame time a couple of times a second to reduce flashing
  if (frame_time_reported.elapsed()>500)
    {    
      //! \todo Frame time calculation is wrong... need -1 correction to number of frames
      const float average_time=std::accumulate
	(
	 frame_times.begin(),
	 frame_times.end(),
	 0
	 )/static_cast<float>(frame_times.size());
      
      const float fps=1000.0/average_time;
      
      std::ostringstream report;
      report.setf(std::ios::fixed);
      report.precision(1);
      
      uint n_triangles=0;
      uint n_vertices=0;
      for (uint m=0;m<mesh.size();m++)
	{
	  if (mesh[m])
	    {
	      n_triangles+=mesh[m]->triangles();
	      n_vertices+=mesh[m]->vertices();
	    }
	}
      report 
	<< "Triangles: " 
	<< n_triangles
	<< ", "
	<< "Vertices: " 
	<< n_vertices 
	<< ", "
	<< "FPS: " 
	<< fps 
	<< "\n";
    
      _notify.notify(report.str());
      frame_time_reported.restart();
    }
}

void TriangleMeshViewerDisplay::initializeGL()
{
  if (_verbose)
    {
      std::cerr << "Double buffering " << (doubleBuffer() ? "ON" : "OFF") << "\n";
      std::cerr << "Auto Buffer Swap " << (autoBufferSwap() ? "ON" : "OFF") << "\n";
      std::cerr << "Multisampling    " << (format().sampleBuffers() ? "ON" : "OFF") << "\n";
    }

  const FloatRGBA bg=background_colour();
  glClearColor(bg.r,bg.g,bg.b,1.0f);

  // Switch depth-buffering on
  glEnable(GL_DEPTH_TEST);

  // Basic lighting stuff (set ambient globally rather than in light)
  GLfloat black_light[]={0.0,0.0,0.0,1.0};
  glLightfv(GL_LIGHT0,GL_AMBIENT,black_light);
  glLightfv(GL_LIGHT0,GL_SPECULAR,black_light);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  // Do smooth shading 'cos colours are specified at vertices
  glShadeModel(GL_SMOOTH);

  // Use arrays of data
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
}

void TriangleMeshViewerDisplay::resizeGL(int w,int h)
{
  width=w;
  height=h;

  if (_verbose)
    std::cerr << "QGLWidget resized to " << width << "x" << height << std::endl;

  glViewport(0,0,static_cast<GLint>(w),static_cast<GLint>(h));
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // View angle is specified in vertical direction, but we need to exaggerate it if image is taller than wide.
  const float view_angle_degrees=minimum(90.0,width>height ? 45.0 : 45.0*height/width);

  gluPerspective
    (
     view_angle_degrees,
     static_cast<float>(width)/static_cast<float>(height),
     0.01,10.0
     );  // Was 0.1 (too far); 0.001 gives artefacts

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void TriangleMeshViewerDisplay::draw_frame(const XYZ& p,const XYZ& l,const XYZ& u,float r,float t)
{
  frame_number++;

  camera_position=p;
  camera_lookat=l;
  camera_up=u;
  object_rotation=r;
  object_tilt=t;

  updateGL();
}
