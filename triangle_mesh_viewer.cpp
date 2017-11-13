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

#include "triangle_mesh_viewer.h"

/*! The viewer will be parented on the specified widget, 
  but with a Qt::Window flag to make it a top-level window
 */
TriangleMeshViewer::TriangleMeshViewer(QWidget* parent,const ParametersRender* param,const std::vector<const TriangleMesh*>& mesh,bool verbose)
  :QWidget(parent,Qt::Window)
  ,_verbose(verbose)
  ,parameters(param)
  ,camera_position(0.0f,-3.0f,0.0f)
  ,camera_forward(0.0f,1.0f,0.0f)
  ,camera_up(0.0f,0.0f,1.0f)
  ,camera_velocity(0.0f)
  ,camera_yaw_rate(0.0f)
  ,camera_pitch_rate(0.0f)
  ,camera_roll_rate(0.0f)
  ,object_tilt(30.0f*M_PI/180.0f)
  ,object_rotation(0.0f)
  ,object_spinrate(0.0f)
  ,keypressed_arrow_left(false)
  ,keypressed_arrow_right(false)
  ,keypressed_arrow_up(false)
  ,keypressed_arrow_down(false)
  ,keypressed_mouse_left(false)
  ,keypressed_mouse_right(false)
  ,fly_mode(false)
{
  QGridLayout*const grid=new QGridLayout();
  setLayout(grid);
  grid->setRowStretch(0,1);
  grid->setColumnStretch(0,1);

  //! \todo Is there any good reason not to enable multisampling by default ?
  QGLFormat gl_format;
  gl_format.setSampleBuffers(true);

  display=new TriangleMeshViewerDisplay(this,gl_format,param,mesh,_verbose);
  grid->addWidget(display,0,0);

  tilt_box=new QGroupBox("Tilt");
  tilt_box->setLayout(new QVBoxLayout());
  tilt_box->layout()->setAlignment(Qt::AlignCenter);
  tilt_slider=new QSlider(Qt::Vertical);
  tilt_box->layout()->addWidget(tilt_slider);
  tilt_slider->setRange(-80,80);
  tilt_slider->setSingleStep(10);
  tilt_slider->setValue(30);
  tilt_slider->setTickInterval(10);
  tilt_slider->setTickPosition(QSlider::TicksBothSides);
  tilt_slider->setTracking(true);
  grid->addWidget(tilt_box,0,1);

  spinrate_box=new QGroupBox("Spin Rate");
  spinrate_box->setLayout(new QHBoxLayout());
  spinrate_box->layout()->setAlignment(Qt::AlignCenter);
  spinrate_slider=new QSlider(Qt::Horizontal);
  spinrate_box->layout()->addWidget(spinrate_slider);
  spinrate_slider->setRange(-80,80);
  spinrate_slider->setSingleStep(10);
  spinrate_slider->setValue(0);
  spinrate_slider->setTickInterval(10);
  spinrate_slider->setTickPosition(QSlider::TicksBothSides);
  spinrate_slider->setTracking(true);
  grid->addWidget(spinrate_box,1,0);
  
  button_box=new QWidget();
  button_box->setLayout(new QVBoxLayout());
  grid->addWidget(button_box,1,1);

  QPushButton*const fly_button=new QPushButton("Fly");
  button_box->layout()->addWidget(fly_button);
  fly_button->setToolTip("While flying:\nEsc will return to normal view.\nMouse controls pitch and yaw.\nLeft and right mouse buttons (or left/right arrow keys) control roll.\nMouse wheel (or up/down arrow keys) control speed.");

  QPushButton*const reset_button=new QPushButton("Reset");
  button_box->layout()->addWidget(reset_button);
  reset_button->setToolTip("Press to restore initial default orientation.");

  statusbar=new QStatusBar();
  grid->addWidget(statusbar,2,0,1,2);

  //Calling setFocus() when switching to fly mode seems to avoid need for this, but do it anyway.
  tilt_slider->setFocusPolicy(Qt::NoFocus);
  spinrate_slider->setFocusPolicy(Qt::NoFocus);

  connect(
	  tilt_slider,SIGNAL(valueChanged(int)),
	  this,SLOT(set_tilt(int))
	  );
  connect(
	  spinrate_slider,SIGNAL(valueChanged(int)),
	  this,SLOT(set_spinrate(int))
	  );
  connect(
	  fly_button,SIGNAL(clicked()),
	  this,SLOT(fly())
	  );
  connect(
	  reset_button,SIGNAL(clicked()),
	  this,SLOT(reset())
	  );

  clock.reset(new QTime());
  clock->start();
  last_t=0;

  QTimer*const timer=new QTimer(this);  
  connect(timer,SIGNAL(timeout()),this,SLOT(tick()));
  timer->start(static_cast<int>(ceil(1000.0f/parameters->fps_target)));
  
  setMouseTracking(true);  // To get moves regardless of button state
  display->setMouseTracking(true);
  setFocus();
}

TriangleMeshViewer::~TriangleMeshViewer()
{}

void TriangleMeshViewer::notify(const std::string& msg)
{
  notify_message=msg;
}

void TriangleMeshViewer::keyPressEvent(QKeyEvent* e)
{
  if (fly_mode)
    {
      if (e->key()==Qt::Key_Escape) unfly();
      else if (e->key()==Qt::Key_Left) keypressed_arrow_left=true;
      else if (e->key()==Qt::Key_Right) keypressed_arrow_right=true;
      else if (e->key()==Qt::Key_Up) keypressed_arrow_up=true;
      else if (e->key()==Qt::Key_Down) keypressed_arrow_down=true;
      else e->ignore();
    }
  else
    {
      e->ignore();
    }
}

void TriangleMeshViewer::keyReleaseEvent(QKeyEvent* e)
{
  if (fly_mode)
    {
      if (e->key()==Qt::Key_Left) keypressed_arrow_left=false;
      else if (e->key()==Qt::Key_Right) keypressed_arrow_right=false;
      else if (e->key()==Qt::Key_Up) keypressed_arrow_up=false;
      else if (e->key()==Qt::Key_Down) keypressed_arrow_down=false;
      else e->ignore();
    }
  else
    {
      e->ignore();
    }
}

void TriangleMeshViewer::mousePressEvent(QMouseEvent* e)
{
  if (fly_mode)
    {
      if (e->button()==Qt::LeftButton) keypressed_mouse_left=true;
      else if (e->button()==Qt::RightButton) keypressed_mouse_right=true;
      else if (e->button()==Qt::MidButton) camera_velocity=0.0f;
      else e->ignore();
    }
  else
    {
      e->ignore();
    }
}

void TriangleMeshViewer::mouseReleaseEvent(QMouseEvent* e)
{
  if (fly_mode)
    {
      if (e->button()==Qt::LeftButton) keypressed_mouse_left=false;
      else if (e->button()==Qt::RightButton) keypressed_mouse_right=false;
      else e->ignore();
    }
  else
    {
      e->ignore();
    }
}

void TriangleMeshViewer::wheelEvent(QWheelEvent* e)
{
  if (fly_mode)
    {
      camera_velocity+=e->delta()*(0.03125f/480.0f);
    }
  else
    {
      e->ignore();
    }
}

inline float signedsquare(float v) 
{
  return (v<0.0f ? -v*v : v*v);
}

void TriangleMeshViewer::mouseMoveEvent(QMouseEvent* e)
{
  if (fly_mode)
    {
      camera_yaw_rate=4.0f*signedsquare(e->pos().x()/static_cast<float>(size().width())-0.5f);
      camera_pitch_rate=(parameters->joystick_mouse ? 1.0f : -1.0f)*4.0f*signedsquare(e->pos().y()/static_cast<float>(size().height())-0.5f);
    }
  else
    {
      e->ignore();
    }
}

void TriangleMeshViewer::set_mesh(const std::vector<const TriangleMesh*>& m)
{
  if (fly_mode) unfly();
  display->set_mesh(m);
}

void TriangleMeshViewer::fly()
{
  fly_mode=true;
  camera_velocity=0.125f;
  spinrate_slider->setValue(0);
  tilt_box->hide();
  spinrate_box->hide();
  button_box->hide();
  setFocus();
  QCursor::setPos(mapToGlobal(QPoint(width()/2,height()/2)));
}

void TriangleMeshViewer::unfly()
{
  reset();
  tilt_box->show();
  spinrate_box->show();
  button_box->show();
}

void TriangleMeshViewer::reset()
{
  fly_mode=false;
  camera_position=XYZ(0.0f,-3.0f,0.0f);
  camera_forward=XYZ(0.0f,1.0f,0.0f);
  camera_up=XYZ(0.0f,0.0f,1.0f);
  camera_velocity=0.0f;
  camera_yaw_rate=0.0f;
  camera_pitch_rate=0.0f;
  camera_roll_rate=0.0f;
  tilt_slider->setValue(30);
  spinrate_slider->setValue(0);
  object_rotation=0.0f;
  statusbar->clearMessage();
}

void TriangleMeshViewer::tick()
{
  const int t=clock->elapsed();
  const float dt=0.001f*(t-last_t);
  last_t=t;

  camera_roll_rate=0.0f;
  if (keypressed_arrow_left || keypressed_mouse_left) camera_roll_rate+=0.5f;
  if (keypressed_arrow_right || keypressed_mouse_right) camera_roll_rate-=0.5f;

  if (keypressed_arrow_up) camera_velocity+=120.0f*(0.03125f/480.0f);
  if (keypressed_arrow_down) camera_velocity-=120.0f*(0.03125f/480.0f);
  
  //! \todo Replace cheesy rotation hacks with proper rotation matrices

  XYZ camera_right=(camera_forward*camera_up).normalised();

  const XYZ camera_right_rolled=camera_right+(dt*camera_roll_rate)*camera_up;
  const XYZ camera_up_rolled=camera_up-(dt*camera_roll_rate)*camera_right;

  camera_right=camera_right_rolled.normalised();
  camera_up=camera_up_rolled.normalised();

  camera_forward=(camera_forward+dt*camera_yaw_rate*camera_right+dt*camera_pitch_rate*camera_up).normalised();
  camera_up=(camera_right*camera_forward).normalised();

  camera_position+=(dt*camera_velocity)*camera_forward;
  object_rotation+=object_spinrate*dt;

  if (display->isVisible())
    {
      display->draw_frame(camera_position,camera_position+camera_forward,camera_up,object_rotation,object_tilt);
    }

  std::ostringstream msg;
  if (fly_mode)
    {
      msg << "Velocity:" << camera_velocity << "  Roll rate:" << camera_roll_rate << "  ";
    }
  msg << notify_message;
  statusbar->showMessage(msg.str().c_str());
}

void TriangleMeshViewer::set_tilt(int v)
{
  object_tilt=v*(M_PI/180.0);
}

void TriangleMeshViewer::set_spinrate(int v)
{
  object_spinrate=v*(M_PI/180.0);
}
