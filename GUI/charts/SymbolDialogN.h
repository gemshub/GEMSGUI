//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file SymbolDialog.h
/// Declaration of class SymbolDialog dialog to set up SeriesLineData description
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

#ifndef SymbolDialog_included
#define SymbolDialog_included

#include <QDialog>
#include <QButtonGroup>
#include "graph_data.h"

namespace Ui {
class SymbolDialogData;
}

namespace jsonui {

/// \class SymbolDialog - dialog to edit one plot line descriptions
class SymbolDialog : public QDialog
{
    Q_OBJECT

protected slots:
    virtual void CmSelectColor();

public:

    SymbolDialog(const SeriesLineData& aData, QWidget* parent = nullptr );
    virtual ~SymbolDialog();

    SeriesLineData& GetPlotLine();

private:

    SeriesLineData linedata;
    Ui::SymbolDialogData *ui;
    QButtonGroup *smbGroup;

};

} // namespace jsonui

#endif // SymbolDialog_included
