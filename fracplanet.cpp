// Source file for fracplanet
// Copyright (C) 2006 Tim Day
/*! \page License License

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*! \mainpage Fracplanet : fractal terrain generator

  \author Tim Day 

  \section introduction Introduction
  "Fracplanet" is an interactive tool for generating fractal planets and terrains.
  It can output the generated meshes to files suitable for use by POV-Ray.

  \todo For new features to be added, see the TODO file.
 */

#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <qapplication.h>

#include "fracplanet_main.h"

//! Application code
/*! Currently this simply creates a TriangleMesh object of some sort,
  then passes it to a viewer.
 */
int main(int argc,char* argv[])
{
  QApplication app(argc,argv);

  try
    {

      boost::program_options::options_description opt_desc("Recognised options");
      opt_desc.add_options()
	("display-list,d","display list rendering")
	("help,h","show list of recognised options")
	("wireframe,w","wireframe mode");

      boost::program_options::variables_map opt_map;
      boost::program_options::store
	(
	 boost::program_options::parse_command_line(argc,argv,opt_desc),
	 opt_map
	 );
      boost::program_options::notify(opt_map);

      if (opt_map.count("help"))
	{
	  std::cerr << opt_desc << std::endl;
	  return 1;
	}

      const bool wireframe=opt_map.count("wireframe");
      const bool display_list=opt_map.count("display-list");
    }
  catch (boost::program_options::error e)
    {
      std::cerr << "Bad command line: " << e.what() << std::endl;
      std::cerr << "Use -h or --help to list recognised options" << std::endl;
      return 1;
    }
  FracplanetMain*const main_widget=new FracplanetMain(0,&app);

  app.setMainWidget(main_widget);
  main_widget->show();

  std::cerr << "sizeof(ByteRGBA) is " << sizeof(ByteRGBA) << " (4 is good)\n";  
  std::cerr << "sizeof(Vertex) is " << sizeof(Vertex) << " (32 is good)\n";

  //  std::cerr << "GL Extensions are :\n\"" << glGetString(GL_EXTENSIONS) << "\"\n";
  //std::cerr << "GLU Extensions are :\n\"" << gluGetString(GL_EXTENSIONS) << "\"\n";

  std::cerr << "Commencing main loop...\n";

  return app.exec();
}
