/****************************************************************************
** Form interface generated from reading ui file 'HLinpDialog.ui'
**
** Created: Втр Лют 5 12:33:54 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef HLINPDIALOGDATA_H
#define HLINPDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QListView;
class QListViewItem;
class QPushButton;

class HLinpDialogData : public QDialog
{ 
    Q_OBJECT

public:
    HLinpDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~HLinpDialogData();

    QPushButton* btnHelp;
    QPushButton* btnPrint;
    QPushButton* btnUndo;
    QPushButton* btnBCC;
    QPushButton* btnClose;
    QPushButton* btnCancel;
    QListView* ListView1;


protected slots:
    virtual void CmBCC();
    virtual void CmCancel();
    virtual void CmHelp();
    virtual void CmOK();
    virtual void CmPrint();
    virtual void CmUndo();

protected:
    QHBoxLayout* hbox;
};

#endif // HLINPDIALOGDATA_H
