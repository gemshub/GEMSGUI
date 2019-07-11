
#include "plot_model.h"

QVariant PlotModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    switch( role )
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return m_plot.getValue( index.row(), index.column() );
    default: break;
    }
    return QVariant();
}

QVariant PlotModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        if( orientation == Qt::Horizontal )
            return m_plot.getColumnName( section );
        else
            return QVariant(section);

    case Qt::TextAlignmentRole:
        return int(Qt::AlignRight | Qt::AlignVCenter);
    default:
        break;
    }

    return QVariant();
}

void PlotModel::resetMatrixData()
{
    beginResetModel();
    endResetModel();
}
