/****************************************************************************
** Form interface generated from reading ui file 'HLresDialog.ui'
**
** Created: Fri Nov 16 14:53:58 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef HLRESDIALOGDATA_H
#define HLRESDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QListView;
class QListViewItem;
class QPushButton;

class HLresDialogData : public QDialog
{ 
    Q_OBJECT

public:
    HLresDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~HLresDialogData();

    QPushButton* btnHelp;
    QPushButton* btnUndo;
    QPushButton* btnPrint;
    QPushButton* btnClose;
    QListView* ListView1;


protected slots:
    virtual void CmClose();
    virtual void CmHelp();
    virtual void CmPrint();
    virtual void CmRefresh();

protected:
    QHBoxLayout* hbox_2;
};

#endif // HLRESDIALOGDATA_H
