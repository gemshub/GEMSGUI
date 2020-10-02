//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file LegendDialog.cpp
/// Implementation of class LegendDialog dialog to set up
/// plotting system description
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
#include <QFontDialog>
#include "ui_LegendDialog4N.h"
#include "LegendDialogN.h"
#include "GraphDialogN.h"
#include "graph_data.h"
using namespace std;

namespace jsonui {

LegendDialog::LegendDialog( GraphDialog* aGraph ):
    QDialog(aGraph), ui(new Ui::LegendDialogData),
    graph((aGraph->gr_data.get())), graphDlg(aGraph)
{
    ui->setupUi(this);

    std::string cap = "Customize Graph: ";
    cap += graph->title;
    setWindowTitle( cap.c_str() );
    setChartData();

    //ui->comboBox1->hide();
    connect( ui->comboBox1, SIGNAL(activated/*highlighted*/(int)), SLOT(CmChangeGraphType(int)) );
    connect( ui->pBtnChangeFont, SIGNAL(clicked()), this, SLOT(CmChangeLabelFont()));
    connect( ui->buttomColor, SIGNAL(clicked()), this, SLOT(CmBackground()));

    QObject::connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(CmOk()));
    QObject::connect( ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect( ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(CmApply()));
    QObject::connect( ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(CmHelp()));
}


LegendDialog::~LegendDialog()
{
    delete ui;
}


void  LegendDialog::CmChangeGraphType( int /*new_type*/ )
{

}

void LegendDialog::CmOk()
{
    if( !applyToDialog() )
        return;
    accept();
}

void LegendDialog::CmApply()
{
    applyToDialog();
}

void LegendDialog::CmChangeLabelFont()
{
    bool ok;
    QFont selected_font = QFontDialog::getFont(&ok, labelFont, this);
    if( ok )
    {
        labelFont = selected_font;
        ui->pLabelFont->setText( labelFont.toString() );
    }
}

void LegendDialog::CmBackground()
{
    QColor cl = QColorDialog::getColor(backgroundColor, this);

    if( cl.isValid() )
    {
        QPalette pl = ui->pColor->palette();
        pl.setColor( QPalette::Window, cl );
        ui->pColor->setPalette(pl);
        backgroundColor = cl;
    }
}

void LegendDialog::CmHelp()
{
    helpWin( "LegendDialogJsonui", "" );
}


//------------------------------------------------------------------------------

void LegendDialog::setChartData()
{
    ui->pTitle->setText(graph->title.c_str());

    oldGraphType = graph->getGraphType();
    ui->comboBox1->setCurrentIndex( graph->getGraphType() );

    ui->pAxisX->setValue( graph->axisTypeX );
    ui->pAxisY->setValue( graph->axisTypeY );

    ui->pXname->setText( graph->xName.c_str() );
    ui->pYname->setText( graph->yName.c_str() );

    QString str;
    ui->pX0->setValidator( new QDoubleValidator( ui->pX0 ) );
    ui->pX0->setText( str.setNum (graph->region[0]) );

    ui->pXn->setValidator( new QDoubleValidator( ui->pXn ) );
    ui->pXn->setText( str.setNum (graph->region[1]) );

    ui->pY0->setValidator( new QDoubleValidator( ui->pY0 ) );
    ui->pY0->setText( str.setNum (graph->region[2]) );

    ui->pYn->setValidator( new QDoubleValidator( ui->pYn ) );
    ui->pYn->setText( str.setNum (graph->region[3]) );

    ui->pfX0->setValidator( new QDoubleValidator( ui->pfX0 ) );
    ui->pfX0->setText( str.setNum (graph->part[0]) );

    ui->pfXn->setValidator( new QDoubleValidator( ui->pfXn ) );
    ui->pfXn->setText( str.setNum (graph->part[1]) );

    ui->pfY0->setValidator( new QDoubleValidator( ui->pfY0 ) );
    ui->pfY0->setText( str.setNum (graph->part[2]) );

    ui->pfYn->setValidator( new QDoubleValidator( ui->pfYn ) );
    ui->pfYn->setText( str.setNum (graph->part[3]) );

    backgroundColor = graph->getBackgroundColor();
    ui->pColor->setAutoFillBackground( true);
    QPalette pl = ui->pColor->palette();
    pl.setColor( QPalette::Window, backgroundColor );
    ui->pColor->setPalette(pl);

    labelFont = graph->axisFont;
    ui->pLabelFont->setText(labelFont.toString());
}


bool LegendDialog::applyToDialog()
{
    double x0 =  ui->pX0->text().toDouble();
    double xn =  ui->pXn->text().toDouble();
    double y0 =  ui->pY0->text().toDouble();
    double yn =  ui->pYn->text().toDouble();

    double fx0 =  ui->pfX0->text().toDouble();
    double fxn =  ui->pfXn->text().toDouble();
    double fy0 =  ui->pfY0->text().toDouble();
    double fyn =  ui->pfYn->text().toDouble();

    graph->title = ui->pTitle->text().toStdString();
    graph->setGraphType( ui->comboBox1->currentIndex() );
    graph->axisTypeX = ui->pAxisX->value();
    graph->axisTypeY = ui->pAxisY->value();
    graph->xName = ui->pXname->text().toStdString();
    graph->yName = ui->pYname->text().toStdString();

    graph->region[0] = min(x0, xn);
    graph->region[1] = max(x0, xn);
    graph->region[2] = min(y0, yn);
    graph->region[3] = max(y0, yn);

    graph->part[0] = min(fx0, fxn);
    graph->part[1] = max(fx0, fxn);
    graph->part[2] = min(fy0, fyn);
    graph->part[3] = max(fy0, fyn);

    graph->axisFont = labelFont;
    graph->setBackgroundColor(backgroundColor);

    graphDlg->UpdateAll(nullptr);
    emit graphDlg->dataChanged( graph );

    return true;
}


} // namespace jsonui

//--------------------- End of LegendDialog.cpp ---------------------------

