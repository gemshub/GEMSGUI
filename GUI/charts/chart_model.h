//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file chart_model.h
/// Declaration of the ChartDataModel class -  vertical model for line,
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

#ifndef CHARTDATAMODEL_H
#define CHARTDATAMODEL_H

#include <iostream>
#include <QJsonObject>
#include <QtCore/QAbstractTableModel>

#ifndef NO_JSONIO
namespace jsonio {
class JsonDom;
}
#endif

namespace jsonui {

/// \class ChartDataModel
/// \brief The ChartDataModel class is a vertical model for line,
///  spline, and scatter series.
///
/// Model enable using a data model derived from the QAbstractTableModel
/// class as a data source for a chart. First column of model used to iterate
/// by index

class ChartDataModel : public QAbstractTableModel
{
    Q_OBJECT

public Q_SLOTS:
    // for the model
    void modelUpdated(QModelIndex topLeft, QModelIndex bottomRight);
    void modelRowsAdded(QModelIndex parent, int start, int end);
    void modelRowsRemoved(QModelIndex parent, int start, int end);
    void modelColumnsAdded(QModelIndex parent, int start, int end);
    void modelColumnsRemoved(QModelIndex parent, int start, int end);

signals:
    void changedXSelections();
    void changedYSelections( bool updateNames );

public:

    explicit ChartDataModel( QAbstractTableModel* tableModel, QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setGraphType( int type )
    {
       graphType   = type;
    }

    // working with X, Y columns selection

    int getXColumn( int line ) const
    {
       if( line == -2 )
         return line;

       if( line>=0 && line < static_cast<int>(xcolumns.size()) )
        return xcolumns[static_cast<size_t>(line)];
       else
        return -1;
    }

    /// Get number of Abscissa
    int getAbscissaNumber() const
    {
        return static_cast<int>(xcolumns.size());
    }

    QString abscissaIndexName( int ndx ) const
    {
      if( ndx == -2 )
        return QString("Off");
      if( ndx == -1 )
        return QString("№");
      return QString("%1").arg(ndx);
    }

    int indexAbscissaName( const QString ndxName ) const
    {
      if( ndxName == QString("Off") )
        return -2;
      if( ndxName == QString("№") )
        return -1;
      return ndxName.toInt();
    }

    /// Get list of Abscissa indexes to qComboBox
    QStringList getAbscissaIndexes() const
    {
      QStringList lst;
      lst << abscissaIndexName( -2 );
      lst << abscissaIndexName( -1 );
      for( int ii=0; ii<getAbscissaNumber(); ii++ )
          lst <<  abscissaIndexName( ii );
      return lst;
    }

    /// Get number of series
    size_t getSeriesNumber() const
    {
        return ycolumns.size();
    }

    int getYColumn( size_t line ) const
    {
        return ycolumns[line];
    }

    void setXColumns( const std::vector<int>& axcolumns )
    {
      xcolumns.clear();
      for( auto cl:axcolumns )
       appendXColumn( cl );
      emit changedXSelections();
    }

    void setYColumns( const std::vector<int>& aycolumns,  bool updateNames  )
    {
        ycolumns.clear();
        for( auto cl:aycolumns )
         appendYColumn( cl );
        emit changedYSelections( updateNames );
    }

    std::string getName( size_t line ) const
    {
//        return std::to_string(line);
       return m_model->headerData( getYColumn( line ) , Qt::Horizontal, Qt::DisplayRole ).toString().toStdString();
    }

    /// List of the column of the model that contains the x-coordinates of data points
    const std::vector<int>& XColumns() const
    { return xcolumns; }
    /// List of the column of the model that contains the y-coordinates of data points
    const std::vector<int>& YColumns() const
    { return ycolumns; }


#ifndef NO_JSONIO
    void toJsonNode( jsonio::JsonDom *object ) const;
    void fromJsonNode( const jsonio::JsonDom *object );
#endif

    void toJsonObject(QJsonObject& json) const;
    void fromJsonObject(const QJsonObject& json);

protected:

    /// Extern Table wir
    QAbstractTableModel *m_model;
    int graphType = 0;      ///< GRAPHTYPES ( 0-line by line, 1- cumulative ... )

    /// List of the column of the model that contains the x-coordinates of data points
    std::vector<int> xcolumns;
    /// List of the column of the model that contains the y-coordinates of data points
    std::vector<int> ycolumns;

    QModelIndex mIndex(int row, int column ) const
    {
      if(column == 0 )
        return QModelIndex();
      else
        return m_model->index( row, column-1 );
    }

    QModelIndex mIndex(const QModelIndex &index) const
    {
      return mIndex( index.row(), index.column() );
    }

    QModelIndex indexM(const QModelIndex &m_index) const
    {
        if( m_index.isValid() )
           return index( m_index.row(), m_index.column()+1 );
        return QModelIndex();
    }

    void appendXColumn( int xclm )
    {
      if( xclm >= 0 && xclm < m_model->columnCount() )
        xcolumns.push_back(xclm);
    }

    void appendYColumn( int yclm )
    {
      if( yclm >= 0 && yclm < m_model->columnCount() )
        ycolumns.push_back(yclm);
    }

};

} // namespace jsonui

#endif // CHARTDATAMODEL_H
