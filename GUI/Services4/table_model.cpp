
#include <QItemSelection>
#include <QHeaderView>
#include <QMenu>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>

#include "table_model.h"
#include "v_mod.h"
#include "v_module.h"
#include "model_w.h"
#include "service.h"

QStringList KeyModel::split_key(const char *add_key )
{
    QStringList list;
    TDBKey dbKey( rt[iRt]->GetDBKey() );
    dbKey.SetKey(add_key);
    for( int ii=0; ii < dbKey.KeyNumFlds(); ii++)
        list.append(  char_array_to_string( dbKey.FldKey(ii), dbKey.FldLen( ii )).c_str() );
    return list;

}

std::string KeyModel::line_key( const QStringList& table_row ) const
{
    std::string key;
    TDBKey dbKey( rt[iRt]->GetDBKey() );
    for( int ii=0; ii < dbKey.KeyNumFlds(); ii++)
        dbKey.SetFldKey(ii, table_row[ii].toUtf8().data());
    key += dbKey.UnpackKey();
    return key;
}

void KeyModel::set_old_selection(const TCStringArray &sel)
{
    TDBKey dbKey( rt[iRt]->GetDBKey() );
    selected_species.clear();
    for( const auto& key_data: sel )
        selected_species.append( split_key( key_data.c_str() ));
}

void KeyModel::set_full_list()
{
    TCIntArray temp;
    TCStringArray keyList;
    all_species.clear();

    rt[iRt]->GetKeyList( key_filter.c_str(), keyList, temp);
    for( const auto& rec_key: keyList)
        all_species.append( split_key(rec_key.c_str()) );
}

TCStringArray KeyModel::selected_keys(const std::set<int> &rows) const
{
    TCStringArray sel_keys;
    if( full_list_selection )
    {
        for( const auto& sel_row: rows)
            sel_keys.push_back( line_key(all_species[sel_row]) );
    }
    else
    {
        for( const auto& key_row: selected_species)
            sel_keys.push_back( line_key(key_row) );
    }
    return sel_keys;
}

QItemSelection KeyModel::get_full_list_selection( TCStringArray&  not_found_keys )
{
    int row = 0, ii=0;
    bool found = false;
    QItemSelection selitems;

    not_found_keys.clear();
    for( const auto& selected_key:  selected_species )
    {
        row = 0;
        found = false;
        for(const auto& all_key: all_species )
        {
            for( ii=0; ii<compare_size(); ++ii )
            {
                if( selected_key[ii] != all_key[ii] )
                    break;
            }
            if( ii >= compare_size() )
            {
                selitems.merge( QItemSelection(index(row, 0), index(row, 0)), QItemSelectionModel::Select);
                found = true;
                break;
            }
            row++;
        }
        if( !found )
            not_found_keys.push_back( line_key(selected_key));
    }
    return selitems;
}

QVariant KeyModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    switch( role )
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if( full_list_selection )
            return all_species[index.row()][index.column() ];
        else
            return selected_species[index.row()][index.column() ];
    case Qt::TextAlignmentRole:
    {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    default: break;
    }
    return QVariant();
}

QVariant KeyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
 if( role == Qt::DisplayRole  && orientation == Qt::Horizontal && section < headers.size() )
       return headers[section];
  return QAbstractTableModel::headerData(section, orientation, role);
}

// -----------------------------------------------



RDKeyModel::RDKeyModel(QObject *parent, const char *akey_filter, short NsuT):
    KeyModel(parent, RT_DCOMP,  akey_filter),
    NsuT_selection( NsuT>0)
{ }

QStringList RDKeyModel::add_key(char type, const char *unpack_key)
{
    auto list = split_key( unpack_key );
    list.insert( 0, QString(type) );
    return list;
}

std::string RDKeyModel::line_key( const QStringList& table_row ) const
{
    std::string key = table_row[0].toStdString()+" ";
    TDBKey dbKey( rt[RT_DCOMP]->GetDBKey() );
    for( int ii=0; ii < dbKey.KeyNumFlds(); ii++)
        dbKey.SetFldKey(ii, table_row[ii+1].toUtf8().data());
    key += dbKey.UnpackKey();
    return key;
}

void RDKeyModel::set_old_selection(const TCStringArray &sel)
{
    selected_species.clear();
    for( const auto& key_data: sel )
      selected_species.append( add_key(key_data[0], key_data.c_str()+2 ));
}

void RDKeyModel::set_full_list()
{
    TCIntArray temp;
    TCStringArray keyList;
    all_species.clear();

    // ReacDC list
    if( NsuT_selection )  // template for adsorption
        key_filter[0] = CP_SSPC;
    rt[RT_REACDC]->GetKeyList( key_filter.c_str(), keyList, temp);
    for( const auto& rec_key: keyList)
        all_species.append( add_key(SRC_REACDC, rec_key.c_str()) );

    // Dcomp list
    if( NsuT_selection )  // template for adsorption
        key_filter[0] = CP_SOLID;
    rt[RT_DCOMP]->GetKeyList( key_filter.c_str(), keyList, temp);
    for( const auto& rec_key: keyList)
        all_species.append( add_key(SRC_DCOMP, rec_key.c_str()) );
}

void FixedKeyModel::define_all_list(const TCStringArray &keys_to_select)
{
    all_species.clear();
    for( const auto& rec_key: keys_to_select)
        all_species.append( split_key(rec_key.c_str()) );
}

//-------------------------------------------------------------------------------

KeysTableProxy::KeysTableProxy(QWidget *parent):
    QTableView( parent )
{
    verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    setSelectionMode( sel_mode );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSortingEnabled( true );
    setCornerButtonEnabled( false );
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotPopupContextMenu(QPoint)));
}

void KeysTableProxy::setModel(QAbstractItemModel *model)
{
    SortFilterProxyModel *proxyModel = new SortFilterProxyModel();
    proxyModel->setSourceModel( model );
    QTableView::setModel(proxyModel);
}

TCStringArray KeysTableProxy::allSelectedKeys() const
{
    std::set<int> sel_rows = allSelectedRows();
    TCStringArray sel_keys;
    auto table_model = source_model();
    if( table_model )
        sel_keys= table_model->selected_keys(sel_rows);
    return sel_keys;
}

void  KeysTableProxy::set_selection( QWidget *wdparent )
{
    TCStringArray  not_found_keys;
    KeyModel* table_model = nullptr;
    auto proxyModel = dynamic_cast<SortFilterProxyModel *>(model());
    if( proxyModel )
        table_model = dynamic_cast<KeyModel*>( proxyModel->sourceModel() );
    if( table_model )
    {
        auto selitems =  table_model->get_full_list_selection(not_found_keys);
        selitems =  proxyModel->mapSelectionFromSource(selitems);
        selectionModel()->select( selitems, QItemSelectionModel::Rows|QItemSelectionModel::ClearAndSelect );
    }
    if( !not_found_keys.empty() )
    {
        std::string msg = "Record keys: \n\n";
        for( const auto& key: not_found_keys)
            msg += key + "\n";
        msg += "\ncannot be found in database files! \n";
        msg += "\nCancel wizard and use Database Files menu \n";
        msg += " or proceed selecting different record keys.";
        vfMessage( wdparent, "", msg);
    }
}


void KeysTableProxy::setFilter( QWidget *wdparent, const char *new_filter )
{
    TCStringArray  not_found_keys;
    auto table_model = source_model();
    if( !table_model /*|| !table_model->is_full_list_mode()*/ )
        return;
    auto old_selection = allSelectedKeys();
    table_model->set_old_selection(old_selection);
    table_model->set_filter(new_filter);
    if( table_model->is_full_list_mode() )
    {
        set_selection(wdparent);
    }
}

void KeysTableProxy::setFullSelection( QWidget *wdparent, bool select_mode)
{
    TCStringArray  not_found_keys;
    auto table_model = source_model();
    if( !table_model )
        return;
    if( table_model->is_full_list_mode() && !select_mode  )
    {
        auto old_selection = allSelectedKeys();
        table_model->set_old_selection(old_selection);
    }
    setSortingEnabled( select_mode );
    table_model->set_full_selection(select_mode);
    if( select_mode )
    {
        setSelectionMode( sel_mode );
        set_selection(wdparent);
    }
    else
    {
        setSelectionMode( QAbstractItemView::NoSelection );
    }
}

void KeysTableProxy::setCurrentRow( int row )
{
    if( model()->rowCount() <= 0 )
        return;
    if(  row > model()->rowCount()  )
        row = 0;

    SortFilterProxyModel *proxyModel = dynamic_cast<SortFilterProxyModel *>(model());
    if( proxyModel )
    {
        setCurrentIndex(  proxyModel->mapFromSource( proxyModel->sourceModel()->index(row,0) ) );
        return;
    }
    setCurrentIndex( model()->index(row,0) );
}


int KeysTableProxy::selectedRow() const
{
    if( model()->rowCount() <= 0 || !currentIndex().isValid() )
        return -1;

    SortFilterProxyModel *proxyModel = dynamic_cast<SortFilterProxyModel *>(model());
    if( proxyModel )
        return proxyModel->mapToSource(currentIndex()).row();
    return -1;
}

void KeysTableProxy::selectRows(const std::set<int> &rows)
{
    SortFilterProxyModel *proxyModel = nullptr;
    auto tmodel = model();

    proxyModel = dynamic_cast<SortFilterProxyModel *>(tmodel);
    if( !proxyModel )
        return;
    tmodel =  proxyModel->sourceModel();
    if( !tmodel )
        return;

    QItemSelection selitems;
    int startRow=-1, endRow=-1;
    for( const auto& sel_row: rows )
    {
        if( sel_row >= tmodel->rowCount() )
            break;
        if( startRow == -1 )
            startRow = endRow = sel_row;
        else
            if( endRow+1 == sel_row )
                endRow++;
            else
            {
                selitems.merge( QItemSelection(tmodel->index(startRow, 0), tmodel->index(endRow, 0)), QItemSelectionModel::Select);
                startRow = endRow = sel_row;
            }
    }
    if(startRow != -1 )
        selitems.merge(QItemSelection(tmodel->index(startRow, 0), tmodel->index(endRow, 0)), QItemSelectionModel::Select );

    selitems =  proxyModel->mapSelectionFromSource(selitems);
    selectionModel()->select(selitems, QItemSelectionModel::Rows|QItemSelectionModel::ClearAndSelect );
}

std::set<int> KeysTableProxy::allSelectedRows() const
{
    std::set<int> arr;
    QItemSelection selitems = selectionModel()->selection();
    SortFilterProxyModel *proxyModel = dynamic_cast<SortFilterProxyModel *>(model());
    if( proxyModel )
        selitems =  proxyModel->mapSelectionToSource(selitems);
    QModelIndexList selection = selitems.indexes();

    // Multiple rows can be selected
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        if( index.column() == 0 )
            arr.insert( index.row() );
    }
    return arr;
}

void KeysTableProxy::slotPopupContextMenu(const QPoint &pos)
{
    auto table_model = source_model();
    if( !table_model || table_model->is_full_list_mode()  )
        return;

    std::shared_ptr<QMenu> menu( new QMenu(this) );
    QAction* act;

    act =  new QAction(tr("&Copy"), this);
    act->setShortcut(tr("Ctrl+C"));
    act->setStatusTip(tr("Copy all cells"));
    connect(act, SIGNAL(triggered()), this, SLOT(copyData()));
    menu->addAction(act);

    act =  new QAction(tr("Copy wit&h names"), this);
    act->setShortcut(tr("Ctrl+H"));
    act->setStatusTip(tr("Copy all header&cells"));
    connect(act, SIGNAL(triggered()), this, SLOT(copyDataHeader()));
    menu->addAction(act);

    menu->exec( viewport()->mapToGlobal(pos) );
}

void KeysTableProxy::keyPressEvent(QKeyEvent* e)
{
    auto table_model = source_model();
    if( table_model && !table_model->is_full_list_mode()  )
    {
        if ( e->modifiers() & Qt::ControlModifier )
        {
            switch ( e->key() )
            {
            case Qt::Key_C:
                copyData();
                return;
            case Qt::Key_H:
                copyDataHeader();
                return;
            }
        }
    }
    QTableView::keyPressEvent(e);
}

QString KeysTableProxy::create_header( char split_col_symb )
{
    QString cText;
    QString clipText;
    int col;
    bool frst = true;
    for( col = 0; col < model()->columnCount( rootIndex() ); col++ )
    {
        if( !frst )
            clipText += split_col_symb;
        frst = false;
        cText = model()->headerData( col, Qt::Horizontal, Qt::DisplayRole ).toString();
        if( cText.isEmpty() /* == emptiness.c_str()*/ )
            cText = "  ";//"\r";
        clipText += cText;
    }
    if( !frst )
        clipText += splitRow;
    return clipText;
}


QString KeysTableProxy::create_string( char split_col_symb )
{
    QModelIndex index = currentIndex();
    QString cText;
    QString clipText;

    for( int row = 0; row < model()->rowCount( rootIndex() ); row++ )
    {
        if( row )
            clipText += splitRow;
        for( int col = 0; col < model()->columnCount( rootIndex() ); col++ )
        {
            QModelIndex wIndex = 	index.sibling( row, col );
            if( col )
                clipText += split_col_symb;
            cText = wIndex.data(Qt::EditRole).toString();
            if( cText.isEmpty() /*cText == emptiness.c_str()*/ )
                cText = "  ";//"\r";
            clipText += cText;
        }
    }
    return clipText;
}

void KeysTableProxy::copyData()
{
    QString clipText = create_string( splitCol );
    QApplication::clipboard()->setText( clipText/*, QClipboard::Clipboard*/ );
}

void KeysTableProxy::copyDataHeader()
{
    QString clipText = create_header( splitCol );
    clipText += create_string( splitCol );
    QApplication::clipboard()->setText( clipText/*, QClipboard::Clipboard*/ );
}



