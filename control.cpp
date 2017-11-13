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

#include "control.h"

Control::Control()
  :QWidget()
{
  setLayout(new QVBoxLayout());
}

Control::~Control()
{}

void Control::pickColour(QPushButton* button,FloatRGBA& colour)
{
  const ByteRGBA col_old(colour);
  QColor qcol_old(col_old.r,col_old.g,col_old.b);
  QColor qcol_new=QColorDialog::getColor(qcol_old,this);
  if (qcol_new.isValid())
    {
      colour=FloatRGBA(ByteRGBA(qcol_new.red(),qcol_new.green(),qcol_new.blue(),255));
      
      QPixmap pmap(16,16);
      pmap.fill(qcol_new);
      button->setIcon(QIcon(pmap));
    }
}

/*! Used when initialising colour-chooser buttons.
 */
QIcon Control::build_icon_of_colour(const FloatRGBA& col)
{
  QPixmap pmap(16,16);

  const ByteRGBA bcol(col);
  pmap.fill(QColor(bcol.r,bcol.g,bcol.b));
  return QIcon(pmap);
}

