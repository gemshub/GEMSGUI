#pragma once

#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QTableView>
#include "v_module.h"

///  \class KeyModel
/// class for represents the record keys and is responsible
/// for fetching the data is needed for viewing.
class KeyModel: public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit KeyModel( QObject * parent, size_t irt, const TCStringArray& sel,
                       const char* akey_filter = "*" ):
        QAbstractTableModel(parent),
        iRt(irt),
        full_list_selection( sel.empty() ),
        key_filter( akey_filter )
    {
        set_old_selection( sel );
        if( full_list_selection  )
            set_full_list();
    }
    ~KeyModel() {}

    int rowCount( const QModelIndex& parent ) const
    {
        Q_UNUSED( parent );
        if( full_list_selection )
            return all_species.size();
        else
            return selected_species.size();
    }
    int columnCount ( const QModelIndex& parent  ) const
    {
        Q_UNUSED( parent );
        return rt[iRt]->KeyNumFlds();
    }

    QVariant data ( const QModelIndex & index, int role ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;

    void setHeaders( const QStringList& new_headers )
    {
       headers =new_headers;
    }

    bool is_full_list_mode()
    {
      return  full_list_selection;
    }

    void set_filter( const char* new_filter )
    {
        beginResetModel();
        key_filter = new_filter;
        set_full_list();
        endResetModel();
    }

    void set_full_selection( bool select_mode )
    {
        beginResetModel();
        full_list_selection = select_mode;
        if( full_list_selection && all_species.empty() )
            set_full_list();
        endResetModel();
    }

    QItemSelection get_full_list_selection( TCStringArray&  not_found_keys );
    virtual void set_old_selection( const TCStringArray& sel );
    virtual TCStringArray  selected_keys( const std::set<int>& rows ) const;

protected:

    size_t iRt;
    bool full_list_selection = false;
    std::string key_filter = "*";
    QVector<QStringList> selected_species;
    QVector<QStringList> all_species;
    QStringList headers;

    QStringList split_key( const char* key );
    virtual void set_full_list();
    virtual std::string line_key(const QStringList &table_row) const;
    virtual int compare_size() const
    {
       return rt[iRt]->KeyNumFlds();
    }

};

///  \class RDKeyModel
/// class for represents the ReacDC&DComp keys lists and
/// is responsible for fetching the data is needed for viewing.
class RDKeyModel: public KeyModel
{
    Q_OBJECT

public:

    explicit RDKeyModel( QObject * parent, const TCStringArray& sel, const char* akey_filter = "*", short NsuT=0 );
    ~RDKeyModel() {}

    int columnCount ( const QModelIndex& parent  ) const override
    {
        Q_UNUSED( parent );
        return 5;
    }

    void set_old_selection( const TCStringArray& sel ) override;

protected:

    bool NsuT_selection = false;

    QStringList add_key( char type, const char *key );
    void set_full_list() override;
    std::string line_key(const QStringList &table_row) const override;
    int compare_size() const override
    {
       return 4;
    }
};

/// The SortFilterProxyModel class provides support for sorting
/// and filtering data passed between another model and a view.
class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:

    SortFilterProxyModel( QObject *parent = nullptr ):
        QSortFilterProxyModel(parent)
    { }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override
    {
        return QSortFilterProxyModel::lessThan( left, right);
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override
    {
        if( column < columnCount()-1 )
            QSortFilterProxyModel::sort(column, order);
    }

};

///  \class MatrixTableProxy implements a table view
/// that displays items from a MatrixModel model.
/// Provides support for sorting and filtering data.
class KeysTableProxy: public QTableView
{
    Q_OBJECT

protected slots:
    void slotPopupContextMenu(const QPoint& pos);

public slots:

    void copyData();
    void copyDataHeader();

public:

    explicit KeysTableProxy(  QWidget * parent = nullptr );

    /// Add proxy model
    void setModel(QAbstractItemModel *model) override;

    virtual TCStringArray  allSelectedKeys() const;

    void setFilter( QWidget *parent, const char* new_filter );

    void setFullSelection( QWidget *parent, bool select_mode );

    /// Set&select current row
    virtual void setCurrentRow( int row );
    /// Returns single selection, returns '-1' if nothing selected
    virtual int selectedRow() const;

    /// Set selected rows
    virtual void selectRows( const std::set<int>& rows );
    /// Return all selected rows
    virtual std::set<int> allSelectedRows() const;

protected:

    KeyModel* source_model() const
    {
        KeyModel* table_model = nullptr;
        auto proxyModel = dynamic_cast<SortFilterProxyModel *>(model());
        if( proxyModel )
            table_model = dynamic_cast<KeyModel*>( proxyModel->sourceModel() );
        return table_model;
    }

    void keyPressEvent(QKeyEvent* e) override;

    QString create_string( char split_col_symb );
    QString create_header( char split_col_symb  );
    void set_selection( QWidget *parent );

};

