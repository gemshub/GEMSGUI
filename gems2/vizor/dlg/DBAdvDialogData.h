/****************************************************************************
** Form interface generated from reading ui file 'DBAdvDialog.ui'
**
** Created: Mo 4. Mrz 17:09:09 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DBADVDIALOGDATA_H
#define DBADVDIALOGDATA_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class DBAdvDialogData : public QWidget
{ 
    Q_OBJECT

public:
    DBAdvDialogData( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DBAdvDialogData();

    QFrame* Line1;
    QPushButton* pButtonDUTerm;
    QPushButton* pButtonAdProbe;
    QPushButton* pButtonInteg;
    QLabel* TextLabel1_2;
    QPushButton* pButtonSDref;
    QPushButton* pButtonConst;
    QPushButton* pButtonProfile;
    QPushButton* pButtonSysEq;
    QPushButton* pButtonProcess;
    QPushButton* pButtonGTdemo;
    QPushButton* pButtonBack;
    QPushButton* pButtonHelp;
    QPushButton* pButtonFormats;
    QLabel* tlTitle;
    QLabel* pLogoImg;


protected slots:
    virtual void CmBack();
    virtual void CmHelp();
    virtual void CmFormats();
    virtual void CmOpen_PROCESS();
    virtual void CmOpen_Const();
    virtual void CmOpen_DUTERM();
    virtual void CmOpen_GTDEMO();
    virtual void CmOpen_INTEG();
    virtual void CmOpen_PROBE();
    virtual void CmOpen_PROFILE();
    virtual void CmOpen_SData();
    virtual void CmThermoSystem();

protected:
    QVBoxLayout* vbox2;
    QVBoxLayout* vbox1;
    QHBoxLayout* Layout20;
};

#endif // DBADVDIALOGDATA_H
