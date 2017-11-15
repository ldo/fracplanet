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
  \brief Interface for class TriangleMeshViewerDisplay.
*/

#ifndef _triangle_mesh_viewer_display_h_
#define _triangle_mesh_viewer_display_h_

#include <deque>

#include "common.h"
#include "parameters_render.h"
#include "qtcommon.h"
#include "random.h"
#include "triangle_mesh.h"

class TriangleMeshViewer;

//! Contains the actual rendering functionality of a TriangleMeshViewer.
class TriangleMeshViewerDisplay : public QGLWidget
{
 private:

  Q_OBJECT;

 public:

  //! Constructor.
  TriangleMeshViewerDisplay(TriangleMeshViewer* parent,const QGLFormat& format,const ParametersRender* param,const std::vector<const TriangleMesh*>& m,bool verbose
);

  //! Destructor
  ~TriangleMeshViewerDisplay();

  //! Specify a minimum size
  QSize minimumSizeHint() const;

  //! Guideline size
  QSize sizeHint() const;

  //! Set the mesh being rendered.
  void set_mesh(const std::vector<const TriangleMesh*>& m);

 protected:

  //! Called to repaint GL area.
  void paintGL();

  //! Set up OpenGL.
  void initializeGL();

  //! Deal with resize.
  void resizeGL(int w,int h);
  
 public slots:
  
  //! Called to redisplay scene
  void draw_frame(const XYZ& p,const XYZ& l,const XYZ& u,float r,float t);

 private:

  //! Need to know this to update framerate text
  TriangleMeshViewer& _notify;

  //! Control logging
  const bool _verbose;

  //! The meshes being displayed.
  /*! NB NOT owned here
   */
  std::vector<const TriangleMesh*> mesh;

  //! Pointer to the rendering parameters.
  const ParametersRender* parameters;

  //! GL display list index
  /*! Zero is not a valid value according to red book, so use zero to designate unset */
  uint gl_display_list_index;

  

  //! Frame count.
  uint frame_number;
  
  //! Display area width.
  uint width;

  //! Display area height.
  uint height;

  //! Time frames for FPS measurement.
  QTime frame_time;
  
  //! Time since FPS last reported.
  QTime frame_time_reported;

  //! Queue of frame times to average.
  std::deque<uint> frame_times;

  //@{
  //! Parameter of camera position.
  XYZ camera_position;
  XYZ camera_lookat;
  XYZ camera_up;
  //@}

  //@{
  //! Parameters of object
  float object_tilt;
  float object_rotation;
  //@}

  void check_for_gl_errors(const char*) const;
  
  //! Compute background colour from render parameters and camera height
  const FloatRGBA background_colour() const;
};

#endif
