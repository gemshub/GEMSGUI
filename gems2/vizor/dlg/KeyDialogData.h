/****************************************************************************
** Form interface generated from reading ui file 'KeyDialog.ui'
**
** Created: Do 28. Feb 13:33:29 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KEYDIALOGDATA_H
#define KEYDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;

class KeyDialogData : public QDialog
{ 
    Q_OBJECT

public:
    KeyDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KeyDialogData();

    QListBox* pList;
    QLabel* pLabel;
    QPushButton* PushButton3;
    QPushButton* pFilterButton;
    QPushButton* pButton2;
    QPushButton* pButton3;
    QPushButton* PushButton1;


public slots:
    virtual void CmSelectAll();
    virtual void CmClearAll();

protected slots:
    virtual void CmFilter();

protected:
    QHBoxLayout* Layout2;
};

#endif // KEYDIALOGDATA_H
