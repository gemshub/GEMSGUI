/****************************************************************************
** Form interface generated from reading ui file 'EQDialog.ui'
**
** Created: Fr 1. Mrz 14:05:38 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef EQDIALOGDATA_H
#define EQDIALOGDATA_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class EQDialogData : public QWidget
{ 
    Q_OBJECT

public:
    EQDialogData( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EQDialogData();

    QLabel* TextLabel2;
    QLabel* pLogoImg;
    QLabel* pTitleTL;
    QPushButton* pSysEqButton;
    QPushButton* pProcessButton;
    QPushButton* pGtDemoButton;
    QPushButton* pBackButton;
    QPushButton* pNumCtrlButton;
    QPushButton* pHelpButton;
    QPushButton* pDutermButton;
    QPushButton* pProbeButton;
    QPushButton* pIntegButton;
    QFrame* Line1;
    QFrame* Line10;


protected slots:
    virtual void CmBack();
    virtual void CmHelp();
    virtual void CmOpen_DUTERM();
    virtual void CmOpen_GTDEMO();
    virtual void CmOpen_INTEG();
    virtual void CmOpen_PROBE();
    virtual void CmOpen_PROCESS();
    virtual void CmSystemDialog();
    virtual void CmAdvanced();

protected:
    QVBoxLayout* vbox1;
    QHBoxLayout* hbox;
    QVBoxLayout* vbox2;
};

#endif // EQDIALOGDATA_H
