//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file SymbolDialog.cpp
/// Implementation of class SymbolDialog dialog to set up SeriesLineData description
/// (Plotting system description)
//
// JSONUI is a C++ Qt5-based widget library and API aimed at implementing 
// the GUI for editing/viewing the structured data kept in a NoSQL database,
// with rendering documents to/from JSON/YAML/XML files, and importing/
// exporting data from/to arbitrary foreign formats (csv etc.). Graphical 
// presentation of tabular data (csv format fields) is also possible.
//
// Copyright (c) 2017 Svetlana Dmytriieva (svd@ciklum.com) and
//   Dmitrii Kulik (dmitrii.kulik@psi.ch)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU (Lesser) General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// JSONUI depends on the following open-source software products:
// JSONIO (https://bitbucket.org/gems4/jsonio); Qt5 (https://qt.io).
//

#include <QColorDialog>
#include <QRadioButton>
#include "ui_SymbolDialog4N.h"
#include "SymbolDialogN.h"

namespace jsonui {

SymbolDialog::SymbolDialog( const SeriesLineData& aData, QWidget* parent):
        QDialog( parent ), linedata(aData),
         ui(new Ui::SymbolDialogData)
{
    ui->setupUi(this);

    smbGroup = new QButtonGroup();
    smbGroup->setExclusive(true);

    // make symbol icons by program
    QRadioButton *pp;
    SeriesLineData plot_( "Scale",  0, 40, 0, 0, 0, 60 );
    for(int ii=0; ii<20; ii++ )
    {
      pp = new QRadioButton(ui->pSymbols);
      plot_.setMarkerShape( ii );
      pp->setIcon(markerShapeIcon(plot_));
      smbGroup->addButton(pp, ii);
      ui->gridLayout->addWidget(pp, ii/5, ii%5, 1, 1);
    }

    ui->pColor->setAutoFillBackground( true);
    QPalette pl = ui->pColor->palette();
    pl.setColor( QPalette::Window, linedata.getColor() );
    ui->pColor->setPalette(pl);

    ui->pLineSize->setValue( linedata.getPenSize() );
    ui->pLineStyle->setCurrentIndex( linedata.getPenStyle() );
    ui->pSpline->setChecked( linedata.getSpline() );
    ui->pSymbolSize->setValue( linedata.getMarkerSize() );
    smbGroup->button( linedata.getMarkerShape() )->setChecked( true );

    std::string s  = "Customize plot curve: ";
                s += linedata.getName();
    setWindowTitle(s.c_str());

    QObject::connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect( ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(ui->buttomColor, SIGNAL(clicked()), this, SLOT(CmSelectColor()));
}

SymbolDialog::~SymbolDialog()
{
  delete ui;
}


SeriesLineData& SymbolDialog::GetPlotLine()
{
    int type = smbGroup->checkedId();
    QColor cl = ui->pColor->palette().color(QPalette::Window); //backgroundColor();
    linedata.setChanges( type, ui->pSymbolSize->value(), ui->pLineSize->value(),
                         ui->pLineStyle->currentIndex(), ui->pSpline->isChecked(), cl );
    return linedata;
}

void SymbolDialog::CmSelectColor()
{
    QPalette pl = ui->pColor->palette();
	QColor cl = QColorDialog::getColor(pl.color(QPalette::Background), this);

    if( cl.isValid() )
    {   pl.setColor( QPalette::Background, cl);
        ui->pColor->setPalette(pl);
    }   
}

} // namespace jsonui

//--------------------- End of SymbolDialog.cpp ---------------------------

