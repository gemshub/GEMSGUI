/****************************************************************************
** Form interface generated from reading ui file 'MainDialog.ui'
**
** Created: Fr 1. Mrz 14:05:37 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef MAINDIALOGDATA_H
#define MAINDIALOGDATA_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class MainDialogData : public QWidget
{ 
    Q_OBJECT

public:
    MainDialogData( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MainDialogData();

    QPushButton* pButtonExit;
    QPushButton* pButtonSettings;
    QPushButton* pButtonHelp;
    QPushButton* pButtonAbout;
    QFrame* Line1;
    QPushButton* pButtonDB;
    QPushButton* pButtonEQ;
    QLabel* pLogoImg;
    QLabel* TextLabel1;


protected slots:
    virtual void CmDBDialog();
    virtual void CmEQDialog();
    virtual void CmExit();
    virtual void CmHelp();
    virtual void CmAbout();
    virtual void CmSettings();

protected:
    QHBoxLayout* Layout9;
};

#endif // MAINDIALOGDATA_H
