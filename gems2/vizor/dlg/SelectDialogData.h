/****************************************************************************
** Form interface generated from reading ui file 'SelectDialog.ui'
**
** Created: Mo 4. Mrz 17:09:08 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SELECTDIALOGDATA_H
#define SELECTDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;

class SelectDialogData : public QDialog
{ 
    Q_OBJECT

public:
    SelectDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SelectDialogData();

    QLabel* pLabel;
    QListBox* pList;
    QPushButton* PushButton1;
    QPushButton* pButton2;
    QPushButton* pButton3;
    QPushButton* PushButton4;


protected slots:
    virtual void CmSelectAll();
    virtual void CmClearAll();

protected:
    QVBoxLayout* SelectDialogDataLayout;
    QHBoxLayout* hbox;
};

#endif // SELECTDIALOGDATA_H
