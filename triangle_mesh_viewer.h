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
  \brief Interface for class TriangleMeshViewer.
*/

#ifndef _triangle_mesh_viewer_h_
#define _triangle_mesh_viewer_h_

#include "common.h"
#include "parameters_render.h"
#include "qtcommon.h"
#include "random.h"
#include "triangle_mesh.h"
#include "triangle_mesh_viewer_display.h"

//! A class to display a triangle mesh.
/*! Wraps a TriangleMeshViewerDisplay with some controls.
  \todo Add better controls.
*/
class TriangleMeshViewer : public QWidget
{
 private:

  Q_OBJECT;

 public:

  //! Constructor.
  TriangleMeshViewer(QWidget* parent,const ParametersRender* param,const std::vector<const TriangleMesh*>& m,bool verbose);

  //! Destructor
  ~TriangleMeshViewer();

  //! Used to set message in statusbar
  void notify(const std::string&);

  //! Sets the TriangleMesh to be displayed.
  void set_mesh(const std::vector<const TriangleMesh*>& m);

 public slots:

  void fly();
  void unfly();
  
  void set_tilt(int v);
  void set_spinrate(int v);

 private:

  //! Control logging
  const bool _verbose;

  //! Pointer to the rendering parameters.
  const ParametersRender* parameters;

  //! The actual rendering area.
  TriangleMeshViewerDisplay* display;

  //! Real time for computing how much to advance animation
  boost::scoped_ptr<QTime> clock;

  //! Record time last tick
  int last_t;

  //! Label and box around the elevation slider.
  QGroupBox* tilt_box;

  //! Slider controlling tilt
  QSlider* tilt_slider;
  
  //! Container for fly and reset buttons
  QWidget* button_box;
  
  //! Label and box arond the spin-rate slider.
  QGroupBox* spinrate_box;

  //! Spin rate slider.
  QSlider* spinrate_slider;

  //! Display fly velocity, render info
  QStatusBar* statusbar;

  //! Last notified message
  std::string notify_message;

  //@{
  //! Parameter of camera position.
  XYZ camera_position;
  XYZ camera_forward;
  XYZ camera_up;
  float camera_velocity;
  float camera_yaw_rate;
  float camera_pitch_rate;
  float camera_roll_rate;
  //@}

  //@{
  //! Parameters of object
  float object_tilt;
  float object_rotation;
  float object_spinrate;
  //@}
  
  //@{
  //! Key state
  bool keypressed_arrow_left;
  bool keypressed_arrow_right;
  bool keypressed_arrow_up;
  bool keypressed_arrow_down;
  bool keypressed_mouse_left;
  bool keypressed_mouse_right;
  //@}

  //! Whether in fly mode
  bool fly_mode;

  //! Interested in some key presses
  void keyPressEvent(QKeyEvent* e);

  //! Interested in some key state
  void keyReleaseEvent(QKeyEvent* e);

  //! Interested in mouse clicks for steering
  void mousePressEvent(QMouseEvent* e);

  //! Interested in some button state
  void mouseReleaseEvent(QMouseEvent* e);
  
  //! Interested in mouse position for steering
  void mouseMoveEvent(QMouseEvent* e);

  //! Interested in wheel for speed
  void wheelEvent(QWheelEvent* e);
  
 private slots:

  void tick();

  void reset();
};

#endif
