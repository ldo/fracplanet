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

#include "control_about.h"

#include "license.h"
#include "dialog_documentation.h"

ControlAbout::ControlAbout(QApplication* app)
  :Control()
{
  QLabel*const label0=new QLabel("\nFracplanet - version "+QString(stringify(FRACPLANET_VERSION)));
  layout()->addWidget(label0);
  label0->setAlignment(Qt::AlignHCenter|label0->alignment());
  QFont label0_font(QApplication::font());
  label0_font.setBold(true);
  label0->setFont(label0_font);

  QLabel*const label1=new QLabel("by timday@timday.com\nhttp://fracplanet.sourceforge.net");
  layout()->addWidget(label1);
  label1->setAlignment(Qt::AlignHCenter|label0->alignment());
  QFont label1_font(QApplication::font());
  label1_font.setPointSize(std::max(2,label1_font.pointSize()-1));
  label1->setFont(label1_font);

  DialogDocumentation*const dialog_docs=new DialogDocumentation(this);

  QPushButton*const button_docs=new QPushButton("Show documentation");
  layout()->addWidget(button_docs);
  connect(button_docs,SIGNAL(clicked()),dialog_docs,SLOT(show()));

  QLabel*const label2=new QLabel("Fracplanet License:");
  layout()->addWidget(label2);
  label2->setAlignment(Qt::AlignHCenter|label0->alignment());

  QTextEdit*const license=new QTextEdit();
  layout()->addWidget(license);
  license->setReadOnly(true);
  license->setText(license_string);
 
  QPushButton*const button_about_qt=new QPushButton("About Qt");
  layout()->addWidget(button_about_qt);
  connect(button_about_qt,SIGNAL(clicked()),app,SLOT(aboutQt()));
}
