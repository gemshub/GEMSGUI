//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file chart_model.cpp
/// Implementation of the ChartDataModel class -  vertical model for line,
///  spline, and scatter series
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

#include <QJsonArray>
#include "chart_model.h"
#include "jsonio/jsondom.h"

namespace jsonui {

ChartDataModel::ChartDataModel( QAbstractTableModel *tableModel, QObject *parent) :
    QAbstractTableModel(parent), m_model(tableModel)
{
    //    connect signals from the model
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelUpdated(QModelIndex,QModelIndex)));
    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(modelRowsAdded(QModelIndex,int,int)));
    connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(modelRowsRemoved(QModelIndex,int,int)));
    connect(m_model, SIGNAL(columnsInserted(QModelIndex,int,int)), this, SLOT(modelColumnsAdded(QModelIndex,int,int)));
    connect(m_model, SIGNAL(columnsRemoved(QModelIndex,int,int)), this, SLOT(modelColumnsRemoved(QModelIndex,int,int)));
    connect(m_model, SIGNAL(modelReset()), this, SIGNAL(modelReset()) );
    connect(m_model, SIGNAL(destroyed()), this, SIGNAL(destroyed()));
}

int ChartDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_model->rowCount();
}

int ChartDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_model->columnCount()+1;
}

QVariant ChartDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return "Index";
        else
            return m_model->headerData( section-1, orientation, role );
    } else {
        return m_model->headerData( section, orientation, role );
    }
}

QVariant ChartDataModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if( index.column() == 0 )
           return index.row();
        return m_model->data( mIndex( index), role);
    }
    else if (role == Qt::EditRole)
    {
        if( index.column() > 0 )
           return m_model->data( mIndex( index), role);
    }
    return QVariant();
}

bool ChartDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole && index.column() > 0 )
    {
        m_model->setData( mIndex(index), value, role);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags ChartDataModel::flags(const QModelIndex &index) const
{
    if( index.column() > 0 )
       return m_model->flags( mIndex(index) );
    return QAbstractItemModel::flags(index);
}

// Connect m_model signald

void ChartDataModel::modelUpdated(QModelIndex mtopLeft, QModelIndex mbottomRight)
{
  emit dataChanged(indexM(mtopLeft), indexM(mbottomRight));
}

void ChartDataModel::modelRowsAdded(QModelIndex m_parent, int start, int end)
{
   beginInsertRows(indexM(m_parent), start, end);
   endInsertRows();
}

void ChartDataModel::modelRowsRemoved(QModelIndex m_parent, int start, int end)
{
    beginRemoveRows(indexM(m_parent), start, end);
    endRemoveRows();
}

void ChartDataModel::modelColumnsAdded(QModelIndex m_parent, int start, int end)
{
    beginInsertColumns(indexM(m_parent), start, end);
    endInsertColumns();
}

void ChartDataModel::modelColumnsRemoved(QModelIndex m_parent, int start, int end)
{
    beginRemoveColumns(indexM(m_parent), start, end);
    endRemoveColumns();
}

// ------------------------ work with selection

#ifndef NO_JSONIO

// Writes data to bson
void ChartDataModel::toJsonNode( jsonio::JsonDom *object ) const
{
    auto arr = object->appendArray( "gxclms");
    for(uint ii=0; ii<xcolumns.size(); ii++)
       arr->appendInt( std::to_string(ii), xcolumns[ii] );

    arr = object->appendArray( "gyclms");
    for(uint ii=0; ii<ycolumns.size(); ii++)
       arr->appendInt( std::to_string(ii), ycolumns[ii] );
}

// Reads data from bson
void ChartDataModel::fromJsonNode( const jsonio::JsonDom *object )
{
   std::vector<int> columns;
   object->findArray( "gxclms", columns );
   setXColumns( columns );
    object->findArray( "gyclms", columns );
   setYColumns( columns, false );
}
#endif

void ChartDataModel::toJsonObject(QJsonObject& json) const
{
    QJsonArray xArray;
    for( auto it: xcolumns )
      xArray.append(it);
    json["gxclms"] = xArray;
    QJsonArray yArray;
    for( auto it: ycolumns )
      yArray.append(it);
    json["gyclms"] = yArray;
}


void ChartDataModel::fromJsonObject(const QJsonObject& json)
{
    std::vector<int> columns;
    QJsonArray array = json["gxclms"].toArray();
    for(int ii=0; ii<array.size(); ii++)
        columns.push_back(array[ii].toInt());
    setXColumns( columns );
    columns.clear();
    array = json["gyclms"].toArray();
    for(int ii=0; ii<array.size(); ii++)
        columns.push_back(array[ii].toInt());
    setYColumns( columns, false );
}

} // namespace jsonui
