/****************************************************************************
** Form interface generated from reading ui file 'ProgressDialog_mt.ui'
**
** Created: Sun Sep 9 17:05:51 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PROGRESSDIALOGDATA_H
#define PROGRESSDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QProgressBar;
class QPushButton;

class ProgressDialogData : public QDialog
{ 
    Q_OBJECT

public:
    ProgressDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ProgressDialogData();

    QLabel* TextLabel1;
    QLabel* TextLabel3_3;
    QLabel* TextLabel3_2_2_3;
    QLabel* TextLabel3_2;
    QLabel* pSolid;
    QLabel* pPE;
    QLabel* TextLabel3_2_3;
    QLabel* pGas;
    QLabel* pWater;
    QLabel* TextLabel3_2_2_2_2;
    QLabel* pIT;
    QLabel* pPH;
    QLabel* pIC;
    QLabel* TextLabel3_2_2_2;
    QLabel* TextLabel3_2_2;
    QLabel* TextLabel3;
    QFrame* Line1;
    QPushButton* pStep;
    QPushButton* pClose;
    QFrame* pBottle;
    QProgressBar* pProgress;
    QLabel* pKey;
    QPushButton* pAccept;

protected slots:
    virtual void CmAccept();
    virtual void CmClose();
    virtual void CmStep();

protected:
    QGridLayout* vbox;
    QHBoxLayout* Layout4;
    bool event( QEvent* );
};

#endif // PROGRESSDIALOGDATA_H
