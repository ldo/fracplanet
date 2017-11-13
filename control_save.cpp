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

#include "control_save.h"

#include "fracplanet_main.h"

ControlSave::ControlSave(FracplanetMain* save_target,ParametersSave* param)
  :Control()
  ,parameters(param)
{
  QTabWidget*const tabs=new QTabWidget();
  layout()->addWidget(tabs);

  QWidget*const tab_pov=new QWidget();
  tabs->addTab(tab_pov,"POV-Ray");
  tab_pov->setLayout(new QVBoxLayout());

  QCheckBox*const atmosphere_checkbox=new QCheckBox("Add atmosphere");
  tab_pov->layout()->addWidget(atmosphere_checkbox);
  atmosphere_checkbox->setChecked(parameters->pov_atmosphere);
  atmosphere_checkbox->setToolTip("Select to add an atmosphere");
  connect(
	  atmosphere_checkbox,SIGNAL(stateChanged(int)),
	  this,SLOT(setAtmosphere(int))
	  );

  QCheckBox*const sea_object_checkbox=new QCheckBox("Sea as single object");
  tab_pov->layout()->addWidget(sea_object_checkbox);
  sea_object_checkbox->setChecked(parameters->pov_sea_object);  
  sea_object_checkbox->setToolTip("Select to emit a single object (instead of multiple triangles) for the sea surface");
  connect(
	  sea_object_checkbox,SIGNAL(stateChanged(int)),
	  this,SLOT(setSeaSphere(int))
	  );

  QPushButton*const save_pov=new QPushButton("Save for POV-Ray");
  tab_pov->layout()->addWidget(save_pov);
  save_pov->setToolTip("Press to save object for POV-Ray");
  connect(
	  save_pov,SIGNAL(clicked()),
	  save_target,SLOT(save_pov())
	  );

  QWidget*const tab_blender=new QWidget();
  tabs->addTab(tab_blender,"Blender");
  tab_blender->setLayout(new QVBoxLayout());

  QCheckBox*const per_vertex_alpha=new QCheckBox("Use per-vertex alpha (for clouds)");
  tab_blender->layout()->addWidget(per_vertex_alpha);
  per_vertex_alpha->setChecked(parameters->blender_per_vertex_alpha);
  per_vertex_alpha->setToolTip("Unfortunately Blender seems to ignore alpha components supplied with per-vertex\ncolours so a workround is normally used.\nCheck this box to save as if per-vertex alpha worked.");
  connect(
	  per_vertex_alpha,SIGNAL(stateChanged(int)),
	  this,SLOT(setPerVertexAlpha(int))
	  );

  QPushButton*const save_blender=new QPushButton("Save for Blender");
  tab_blender->layout()->addWidget(save_blender);
  save_blender->setToolTip("Press to save object for Blender");
  connect(
	  save_blender,SIGNAL(clicked()),
	  save_target,SLOT(save_blender())
	  );

  QWidget*const tab_texture=new QWidget();
  tabs->addTab(tab_texture,"Texture");
  tab_texture->setLayout(new QVBoxLayout());

  QCheckBox*const shaded_checkbox=new QCheckBox("Shaded texture");
  tab_texture->layout()->addWidget(shaded_checkbox);
  shaded_checkbox->setChecked(parameters->texture_shaded);
  shaded_checkbox->setToolTip("Check to have the texture include relief shading");
  connect(
	  shaded_checkbox,SIGNAL(stateChanged(int)),
	  this,SLOT(setTextureShaded(int))
	  );

  QWidget*const grid_texture=new QWidget();
  tab_texture->layout()->addWidget(grid_texture);
  QGridLayout* grid_layout=new QGridLayout();
  grid_texture->setLayout(grid_layout);

  grid_layout->addWidget(new QLabel("Texture height",grid_texture),0,0);
  QSpinBox* texture_height_spinbox=new QSpinBox();
  grid_layout->addWidget(texture_height_spinbox,0,1);
  texture_height_spinbox->setMinimum(1);
  texture_height_spinbox->setMaximum(0x7fffffff);
  texture_height_spinbox->setValue(1024);
  texture_height_spinbox->setToolTip("Texture height in pixels; the texture width is the same as the height\nexcept for spherical geometry (planets) when it is double.");
  connect(
	  texture_height_spinbox,SIGNAL(valueChanged(int)),
	  this,SLOT(setTextureHeight(int))
	  );

  QPushButton*const save_texture=new QPushButton("Save as texture");
  tab_texture->layout()->addWidget(save_texture);
  save_texture->setToolTip("Press to save object as textures");
  connect(
	  save_texture,SIGNAL(clicked()),
	  save_target,SLOT(save_texture())
	  );
}

ControlSave::~ControlSave()
{}

void ControlSave::setAtmosphere(int v)
{
  parameters->pov_atmosphere=(v==2);
}

void ControlSave::setSeaSphere(int v)
{
  parameters->pov_sea_object=(v==2);
}

void ControlSave::setPerVertexAlpha(int v)
{
  parameters->blender_per_vertex_alpha=(v==2);
}

void ControlSave::setTextureShaded(int v)
{
  parameters->texture_shaded=(v==2);
}

void ControlSave::setTextureHeight(int v)
{
  parameters->texture_height=v;
}

