/****************************************************************************
** Form interface generated from reading ui file 'SystemDialog.ui'
**
** Created: Втр Лют 5 12:33:38 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SYSTEMDIALOGDATA_H
#define SYSTEMDIALOGDATA_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QFrame;
class QGroupBox;
class QLCDNumber;
class QLabel;
class QPushButton;

class SystemDialogData : public QWidget
{ 
    Q_OBJECT

public:
    SystemDialogData( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~SystemDialogData();

    QLabel* pImg1;
    QLabel* TextLabelTit;
    QPushButton* btnClose;
    QGroupBox* GroupBoxPrint;
    QPushButton* btnPrintTD;
    QPushButton* btnPrintSys;
    QPushButton* btnPrintEqSt;
    QFrame* FrameTPV;
    QLabel* TextLabelT;
    QLabel* TextLabelP;
    QLabel* TextLabelV;
    QLCDNumber* LCDNumberV;
    QLCDNumber* LCDNumberP;
    QLCDNumber* LCDNumberT;
    QLabel* TextLabelRkey;
    QLabel* pRKey;
    QGroupBox* GroupBoxRec;
    QPushButton* btnSysEqNew;
    QPushButton* btnSysEqRem;
    QPushButton* btnSysEqSave;
    QPushButton* btnSysEqLoad;
    QGroupBox* GroupBoxInp;
    QPushButton* btnInpTDview;
    QPushButton* btnInpBCview;
    QPushButton* btnInpPhDCview;
    QGroupBox* GroupBoxRes;
    QPushButton* btnResICview;
    QPushButton* btnResDCview;
    QPushButton* btnDebugView;
    QGroupBox* GroupBoxIPM;
    QPushButton* btnRunIPM;
    QPushButton* btnMoreIPM;
    QPushButton* btnCalcBCC;
    QPushButton* btnMoreBCC;
    QCheckBox* pSimplexCheck;
    QCheckBox* pStepwiseCheck;
    QCheckBox* pRaisedCheck;
    QPushButton* btnHelp;


protected slots:
    virtual void CmBack();
    virtual void CmSelect();
    virtual void CmNew();
    virtual void CmRemake();
    virtual void CmSave();
    virtual void CmRunIPM();
    virtual void CmMoreIPM();
    virtual void CmMoreBCC();
    virtual void CmRunBCC();
    virtual void CmPrintMtparam();
    virtual void CmPrintSystem();
    virtual void CmPrintEqstat();
    virtual void CmHelp();
    virtual void CmOpen_MTPARAM();
    virtual void CmOpen_SYSTEM_ICOMP();
    virtual void CmOpen_inp_PHASE_HL();
    virtual void CmOpen_EQSTAT_ICOMP();
    virtual void CmOpen_res_PHASE_HL();
    virtual void CmOpen_MULTI();

protected:
    QHBoxLayout* Layout5;
    QVBoxLayout* Layout5_2;
    QVBoxLayout* GroupBoxInpLayout;
    QVBoxLayout* GroupBoxResLayout;
};

#endif // SYSTEMDIALOGDATA_H
