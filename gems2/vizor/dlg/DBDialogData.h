/****************************************************************************
** Form interface generated from reading ui file 'DBDialog.ui'
**
** Created: Mo 4. Mrz 11:35:27 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DBDIALOGDATA_H
#define DBDIALOGDATA_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class DBDialogData : public QWidget
{ 
    Q_OBJECT

public:
    DBDialogData( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DBDialogData();

    QLabel* pImg1;
    QPushButton* PushButton3;
    QPushButton* PushButton4;
    QPushButton* PushButton5;
    QLabel* TextLabel1;
    QPushButton* PushButton1_2;
    QPushButton* PushButton2_2;
    QPushButton* PushButton1_2_2;
    QPushButton* PushButton1;
    QPushButton* PushButton2_2_2;
    QPushButton* PushButton2;
    QFrame* Line1;


protected slots:
    virtual void CmAdvanced();
    virtual void CmBack();
    virtual void CmHelp();
    virtual void CmOpen_COMPOS();
    virtual void CmOpen_DCOMP();
    virtual void CmOpen_ICOMP();
    virtual void CmOpen_REACDC();
    virtual void CmOpen_RTPARM();
    virtual void CmOpen_PHASE();

protected:
    QHBoxLayout* hbox;
    QVBoxLayout* vbox;
};

#endif // DBDIALOGDATA_H
