/****************************************************************************
** Form interface generated from reading ui file 'ListFilesDialog.ui'
**
** Created: Mon Nov 19 22:07:41 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef LISTFILESDIALOGDATA_H
#define LISTFILESDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QRadioButton;

class ListFilesDialogData : public QDialog
{ 
    Q_OBJECT

public:
    ListFilesDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ListFilesDialogData();

    QPushButton* pHelpButton;
    QRadioButton* RadioButton1;
    QPushButton* pGO_OKButton;
    QPushButton* pCancelButton;
    QListView* pListFiles;
    QLabel* pLabel;

public slots:
    virtual void CmHelp();

protected:
    QHBoxLayout* Layout3;
    bool event( QEvent* );
};

#endif // LISTFILESDIALOGDATA_H
