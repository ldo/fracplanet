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
  \brief Interface for class FracplanetMain.
*/

#ifndef _fracplanet_main_h_
#define _fracplanet_main_h_

#include "common.h"
#include "control_about.h"
#include "control_render.h"
#include "control_save.h"
#include "control_terrain.h"
#include "parameters_render.h"
#include "parameters_save.h"
#include "parameters_terrain.h"
#include "random.h"

#include "triangle_mesh_cloud.h"
#include "triangle_mesh_terrain.h"
#include "triangle_mesh_viewer.h"

//! Top level GUI component for fracplanet application: contains parameter controls and viewing area
class FracplanetMain : public QWidget,public Progress
{
 private:

  Q_OBJECT

 public:
 
  FracplanetMain(QWidget* parent,QApplication* app,const boost::program_options::variables_map& opts,bool verbose);
  ~FracplanetMain();

  virtual void progress_start(uint target,const std::string&);
  virtual void progress_stall(const std::string& reason);
  virtual void progress_step(uint step);
  virtual void progress_complete(const std::string&);
  
 public slots:
    
  //! Invoked by ControlTerrain to generate new TriangleMesh.
  void regenerate();
  
  //! Invoked by ControlSave to save to file (POV-Ray format).
  void save_pov();

  //! Invoked by ControlSave to save to file (Blender format).
  void save_blender();

  //! Invoked by ControlSave to save to file as texture(s).
  void save_texture();

 private:
  
  //! Control logging.
  const bool _verbose;
 
  QApplication*const application;
  
  //! Owned terrain.
  boost::scoped_ptr<const TriangleMeshTerrain> mesh_terrain;

  //! Owned clouds, if any.
  boost::scoped_ptr<const TriangleMeshCloud> mesh_cloud;

  //! Downcast version for use by mesh viewer.
  std::vector<const TriangleMesh*> meshes;

  ParametersTerrain parameters_terrain;
  ParametersCloud parameters_cloud;
  ParametersRender parameters_render;
  ParametersSave parameters_save;

  ControlTerrain* control_terrain;
  ControlRender* control_render;
  ControlSave* control_save;
  ControlAbout* control_about;

  boost::scoped_ptr<TriangleMeshViewer> viewer;

  QTabWidget* tab;

  uint last_step;

  std::unique_ptr<QProgressDialog> progress_dialog;
  std::string progress_info;
  bool progress_was_stalled;
};

#endif
