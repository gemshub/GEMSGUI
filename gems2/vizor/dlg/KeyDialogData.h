/****************************************************************************
** Form interface generated from reading ui file 'KeyDialog.ui'
**
** Created: Пнд Гру 17 12:43:58 2001
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

    QLabel* pLabel;
    QListBox* pList;
    QPushButton* PushButton1;
    QPushButton* pFilterButton;
    QPushButton* PushButton3;


protected slots:
    virtual void CmFilter();

protected:
    QVBoxLayout* KeyDialogDataLayout;
    QHBoxLayout* Layout6;
};

#endif // KEYDIALOGDATA_H
