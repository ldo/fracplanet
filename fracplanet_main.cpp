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

#include "fracplanet_main.h"

#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "image.h"

FracplanetMain::FracplanetMain(QWidget* parent,QApplication* app,const boost::program_options::variables_map& opts,bool verbose)
  :QWidget(parent)
  ,_verbose(verbose)
  ,application(app)
  ,mesh_terrain(0)
  ,mesh_cloud(0)
  ,parameters_terrain()
  ,parameters_cloud()
  ,parameters_render(opts)
  ,parameters_save(&parameters_render)
  ,last_step(0)
  ,progress_was_stalled(false)
{
  setLayout(new QVBoxLayout);

  tab=new QTabWidget();
  layout()->addWidget(tab);

  control_terrain=new ControlTerrain(this,&parameters_terrain,&parameters_cloud);
  tab->addTab(control_terrain,"Create");

  control_render=new ControlRender(&parameters_render);
  tab->addTab(control_render,"Render");

  control_save=new ControlSave(this,&parameters_save);
  tab->addTab(control_save,"Save");

  control_about=new ControlAbout(application);
  tab->addTab(control_about,"About");
}

FracplanetMain::~FracplanetMain()
{}

void FracplanetMain::progress_start(uint target,const std::string& info)
{
  if (!progress_dialog.get())
    {
      progress_dialog=std::unique_ptr<QProgressDialog>(new QProgressDialog("Progress","Cancel",0,100,this));
      progress_dialog->setWindowModality(Qt::WindowModal);

      progress_dialog->setCancelButton(0);  // Cancel not supported
      progress_dialog->setAutoClose(false); // Avoid it flashing on and off
      progress_dialog->setMinimumDuration(0);
    }

  progress_was_stalled=false;
  progress_info=info;
  progress_dialog->reset();
  progress_dialog->setMaximum(target+1);   // Not sure why, but  +1 seems to avoid the progress bar dropping back to the start on completion
  progress_dialog->setLabelText(progress_info.c_str());
  progress_dialog->show();

  last_step=static_cast<uint>(-1);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  application->processEvents();
}

void FracplanetMain::progress_stall(const std::string& reason)
{
  progress_was_stalled=true;
  progress_dialog->setLabelText(reason.c_str());
  application->processEvents();
}

void FracplanetMain::progress_step(uint step)
{
  if (progress_was_stalled)
    {
      progress_dialog->setLabelText(progress_info.c_str());
      progress_was_stalled=false;
      application->processEvents();
    }

  // We might be called lots of times with the same step.
  // Don't know if Qt handles this efficiently so check for it ourselves.
  if (step!=last_step)
    {
      progress_dialog->setValue(step);
      last_step=step;
      application->processEvents();
    }

  // TODO: Probably better to base call to processEvents() on time since we last called it.
  // (but certainly calling it every step is a bad idea: really slows app down)
}

void FracplanetMain::progress_complete(const std::string& info)
{
  progress_dialog->setLabelText(info.c_str());

  last_step=static_cast<uint>(-1);

  QApplication::restoreOverrideCursor();

  application->processEvents();
}

void FracplanetMain::regenerate()   //! \todo Should be able to retain ground or clouds
{
  const bool first_viewer=!viewer;

  if (viewer)
    {
      viewer->hide();
      viewer.reset();
    }

  meshes.clear();
  mesh_terrain.reset();
  mesh_cloud.reset();

  //! \todo Recreating viewer every time seems like overkill, but Ubuntu (in VM) doesn't seem to like it otherwise.
  viewer.reset(new TriangleMeshViewer(this,&parameters_render,std::vector<const TriangleMesh*>(),_verbose));

  // Tweak viewer appearance so controls not too dominant
  QFont viewer_font;
  viewer_font.setPointSize(viewer_font.pointSize()-2);
  viewer->setFont(viewer_font);
  viewer->layout()->setSpacing(2);
  viewer->layout()->setContentsMargins(2,2,2,2);

  const clock_t t0=clock();

  // There are some issues with type here:
  // We need to keep hold of a pointer to TriangleMeshTerrain so we can call its write_povray method
  // but the triangle viewer needs the TriangleMesh.
  // So we end up with code like this to avoid a premature downcast.
  switch (parameters_terrain.object_type)
    {
    case ParametersObject::ObjectTypePlanet:
      {
    std::unique_ptr<TriangleMeshTerrainPlanet> it(new TriangleMeshTerrainPlanet(parameters_terrain,this));
    meshes.push_back(it.get());
    mesh_terrain.reset(it.release());
    break;
      }
    default:
      {
    std::unique_ptr<TriangleMeshTerrainFlat> it(new TriangleMeshTerrainFlat(parameters_terrain,this));
    meshes.push_back(it.get());
    mesh_terrain.reset(it.release());
    break;
      }
    }

  if (parameters_cloud.enabled)
    {
      switch (parameters_cloud.object_type)
    {
    case ParametersObject::ObjectTypePlanet:
      {
        std::unique_ptr<TriangleMeshCloudPlanet> it(new TriangleMeshCloudPlanet(parameters_cloud,this));
        meshes.push_back(it.get());
        mesh_cloud.reset(it.release());
        break;
      }
    default:
      {
        std::unique_ptr<TriangleMeshCloudFlat> it(new TriangleMeshCloudFlat(parameters_cloud,this));
        meshes.push_back(it.get());
        mesh_cloud.reset(it.release());
        break;
      }
    }
    }

  const clock_t t1=clock();

  progress_dialog.reset(0);

  if (_verbose)
    std::cerr << "Mesh build time was " << (t1-t0)/static_cast<double>(CLOCKS_PER_SEC) << "s" << std::endl;

  viewer->set_mesh(meshes);
  viewer->showNormal();  // showNormal() needed to restore from minimised
  viewer->raise();

  // Only display this first time viewer is created
  if (_verbose && first_viewer)
    {
      std::cerr << "GL info:" << std::endl;
      std::cerr << "  Vendor   : " << glGetString(GL_VENDOR) << std::endl;
      std::cerr << "  Renderer : " << glGetString(GL_RENDERER) << std::endl;
      std::cerr << "  Version  : " << glGetString(GL_VERSION) << std::endl;

      GLint max_elements_vertices;
      GLint max_elements_indices;
      glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,&max_elements_vertices);
      glGetIntegerv(GL_MAX_ELEMENTS_INDICES,&max_elements_indices);
      std::cerr << "  GL_MAX_ELEMENTS_VERTICES : " << max_elements_vertices << std::endl;
      std::cerr << "  GL_MAX_ELEMENTS_INDICES : " << max_elements_indices << std::endl;

      //std::cerr << "  GL Extensions are : \"" << glGetString(GL_EXTENSIONS) << "\"" << std::endl;
    }
}

void FracplanetMain::save_pov()
{
  const QString selected_filename=QFileDialog::getSaveFileName
    (
     this,
     "POV-Ray",
     ".",
     "(*.pov *.inc)"
     );
  if (selected_filename.isEmpty())
    {
      QMessageBox::critical(this,"Fracplanet","No file specified\nNothing saved");
    }
  else if (!(selected_filename.toUpper().endsWith(".POV") || selected_filename.toUpper().endsWith(".INC")))
    {
      QMessageBox::critical(this,"Fracplanet","File selected must have .pov or .inc suffix.");
    }
  else
    {
      viewer->hide();

      const std::string filename_base(selected_filename.left(selected_filename.length()-4).toLocal8Bit());
      const std::string filename_pov=filename_base+".pov";
      const std::string filename_inc=filename_base+".inc";

      const size_t last_separator=filename_inc.rfind('/');
      const std::string filename_inc_relative_to_pov=
    "./"
    +(
      last_separator==std::string::npos
      ?
      filename_inc
      :
      filename_inc.substr(last_separator+1)
      );

      std::ofstream out_pov(filename_pov.c_str());
      std::ofstream out_inc(filename_inc.c_str());

      // Boilerplate for renderer
      out_pov << "camera {perspective location <0,1,-4.5> look_at <0,0,0> angle 45}\n";
      out_pov << "light_source {<100,100,-100> color rgb <1.0,1.0,1.0>}\n";
      out_pov << "#include \""+filename_inc_relative_to_pov+"\"\n";

      mesh_terrain->write_povray(out_inc,parameters_save,parameters_terrain);
      if (mesh_cloud) mesh_cloud->write_povray(out_inc,parameters_save,parameters_cloud);

      out_pov.close();
      out_inc.close();

      const bool ok=(out_pov && out_inc);

      progress_dialog.reset(0);

      viewer->showNormal();
      viewer->raise();

      if (!ok)
    {
      QMessageBox::critical(this,"Fracplanet","Errors ocurred while the files were being written.");
    }
    }
}

void FracplanetMain::save_blender()
  {
    const QString selected_filename=QFileDialog::getSaveFileName
      (
       this,
       "Blender",
       ".",
       "(*.py)"
       );
    if (selected_filename.isEmpty())
      {
        QMessageBox::critical(this,"Fracplanet","No file specified\nNothing saved");
      }
    else
      {
        viewer->hide();

        const std::string filename(selected_filename.toLocal8Bit());
        std::ofstream out(filename.c_str());

        // Boilerplate
        out <<
            "#+\n"
            "# Instructions for loading this model into Blender:\n"
            "#\n"
            "# Open a new Blender document. Get rid of the default cube, and any other\n"
            "# extraneous stuff you don't want. Bring up a Text Editor window.\n"
            "# Click the Open button. Select this .py file to load it into\n"
            "# a new text block. Execute the text block with ALT-P. The recreated model\n"
            "# should now be visible in the 3D view. You can now delete the text.\n"
            "# block containing this script from the document. Save the document.\n"
            "#-\n"
            "\n";
        out <<
            "import bpy\n"
            "import bmesh\n"
            "from mathutils import \\\n"
            "    Color\n"
            "\n"
            "bpy.ops.object.add(type = \"MESH\")\n"
            "the_mesh_obj = bpy.context.active_object\n"
            "the_mesh_obj.name = \"fracplanet\"\n"
            "the_mesh = the_mesh_obj.data\n"
            "the_mesh.name = \"fracplanet\"\n"
            "the_bmesh = bmesh.new()\n"
            "color_layer = the_bmesh.loops.layers.color.new(\"Col\") # same name as used by Blender\n"
            "\n"
            "def v(x, y, z) :\n"
            "  # adds a new vertex to the mesh.\n"
            "    the_bmesh.verts.new((x, y, z))\n"
            "#end v\n"
            "\n";
        out <<
          "def f(material, v0, v1, v2, c0, c1, c2) :\n"
          "  # adds a new face and associated vertex colours to the mesh.\n"
          "    the_bmesh.verts.index_update()\n"
          "    tface = the_bmesh.faces.new((the_bmesh.verts[v0], the_bmesh.verts[v1], the_bmesh.verts[v2]))\n"
          "    tface.smooth = True\n"
          "    tface.material_index = material\n"
          "    colors = {v0 : Color(tuple(c / 255.0 for c in c0[:3])), v1 : Color(tuple(c / 255.0 for c in c1[:3])), v2 : Color(tuple(c / 255.0 for c in c2[:3]))}\n"
                 /* unfortunately, I can't do anything with alpha */
          "    for l in tface.loops :\n"
          "        l[color_layer] = colors[l.vert.index]\n"
          "    #end for\n"
          "#end f\n\n";

        mesh_terrain->write_blender(out,parameters_save,parameters_terrain,"fracplanet");
        if (mesh_cloud) mesh_cloud->write_blender(out,parameters_save,parameters_cloud,"fracplanet");

        out <<
            "the_bmesh.normal_update()\n"
            "the_bmesh.to_mesh(the_mesh)\n"
            "the_mesh.update()\n"
            "bpy.context.scene.update()\n";

        out.close();

        progress_dialog.reset(0);

        viewer->showNormal();
        viewer->raise();

        if (!out)
      {
        QMessageBox::critical(this,"Fracplanet","Errors ocurred while the files were being written.");
      }
      }
  } /*FracplanetMain::save_blender*/

void FracplanetMain::save_texture()
{
  const uint height=parameters_save.texture_height;
  const uint width=height*mesh_terrain->geometry().scan_convert_image_aspect_ratio();

  const QString selected_filename=QFileDialog::getSaveFileName
    (
     this,
     "Texture",
     ".",
     "(*.ppm)"
     );
  if (selected_filename.isEmpty())
    {
      QMessageBox::critical(this,"Fracplanet","No file specified\nNothing saved");
    }
  else if (!(selected_filename.toUpper().endsWith(".PPM")))
    {
      QMessageBox::critical(this,"Fracplanet","File selected must have .ppm suffix.");
    }
  else
    {
      const std::string filename(selected_filename.toLocal8Bit());
      const std::string filename_base(selected_filename.left(selected_filename.length()-4).toLocal8Bit());

      viewer->hide();

      bool ok=true;
      {
    boost::scoped_ptr<Image<ByteRGBA> > terrain_image(new Image<ByteRGBA>(width,height));
    terrain_image->fill(ByteRGBA(0,0,0,0));
    boost::scoped_ptr<Image<ushort> > terrain_dem(new Image<ushort>(width,height));
    terrain_dem->fill(0);

    boost::scoped_ptr<Image<ByteRGBA> > terrain_normals(new Image<ByteRGBA>(width,height));
    terrain_normals->fill(ByteRGBA(128,128,128,0));

    mesh_terrain->render_texture
      (
       *terrain_image,
       terrain_dem.get(),
       terrain_normals.get(),
       parameters_save.texture_shaded,
       parameters_render.ambient,
       parameters_render.illumination_direction()
       );

    if (!terrain_image->write_ppmfile(filename,this)) ok=false;

    if (ok && terrain_dem)
      {
        if (!terrain_dem->write_pgmfile(filename_base+"_dem.pgm",this)) ok=false;
      }

    if (ok && terrain_normals)
      {
        if (!terrain_normals->write_ppmfile(filename_base+"_norm.ppm",this)) ok=false;
      }
      }

      if (ok && mesh_cloud)
    {
      QMessageBox::warning(this,"Fracplanet","Texture save of cloud mesh not currently supported");
      //boost::scoped_ptr<Image<uchar> > cloud_image(new Image<uchar>(width,height));
      //mesh_cloud->render_texture(*cloud_image);
      //if (!cloud_image->write_pgmfile(filename_base+"_cloud.png",this)) ok=false;
    }

      progress_dialog.reset(0);

      viewer->showNormal();
      viewer->raise();

      if (!ok)
    {
      QMessageBox::critical(this,"Fracplanet","Errors ocurred while the files were being written.");
    }
    }
}
