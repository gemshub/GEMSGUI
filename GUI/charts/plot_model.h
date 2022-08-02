#ifndef PLOTMODEL_H
#define PLOTMODEL_H

#include <QJsonObject>
#include <QtCore/QAbstractTableModel>
#include "graph.h"

///  \class TMatrixModel
/// class for represents the data set and is responsible for fetchin
/// the data is neaded for viewing and for writing back any changes.
/// Reading/writing data from/to inherited classes from TAbstractDataContainer interface
class PlotModel: public QAbstractTableModel
{

    Q_OBJECT

    /// Description of plot matrix
    TPlot m_plot;

public:

    PlotModel( const TPlot& plotDesc, QObject * parent = nullptr ):
        QAbstractTableModel(parent), m_plot(plotDesc)
    { }

    ~PlotModel()
    { }

    int rowCount( const QModelIndex& /*parent*/ ) const
    {
        return m_plot.getdX();
    }
    int columnCount ( const QModelIndex& /*parent*/  ) const
    {
        return m_plot.getNAbs()+m_plot.getLinesNumber();
    }

    QVariant data ( const QModelIndex & index, int role ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;

    void resetMatrixData();

    int absCount() const
    {
        return m_plot.getNAbs();
    }

};

#endif // PLOTMODEL_H
