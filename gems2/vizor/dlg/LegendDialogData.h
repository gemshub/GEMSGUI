/****************************************************************************
** Form interface generated from reading ui file 'LegendDialog.ui'
**
** Created: Do 28. Feb 17:25:55 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef LEGENDDIALOGDATA_H
#define LEGENDDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class LegendDialogData : public QDialog
{ 
    Q_OBJECT

public:
    LegendDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~LegendDialogData();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QSpinBox* pAxis;
    QLabel* TextLabel1_2_2;
    QLabel* TextLabel1_2_2_2;
    QLabel* TextLabel1_2_2_3;
    QLabel* TextLabel1_2_2_2_2;
    QLabel* TextLabel1_2;
    QLabel* TextLabel1_2_3;
    QLineEdit* pX0;
    QLineEdit* pXn;
    QLineEdit* pY0;
    QLineEdit* pYn;
    QLineEdit* pfX0;
    QLineEdit* pfXn;
    QLineEdit* pfY0;
    QLineEdit* pfYn;
    QLineEdit* pYname;
    QLineEdit* pXname;
    QPushButton* pBtnApply;
    QPushButton* pBthCancel;
    QPushButton* pBtnBackgr;
    QPushButton* pBtnOk;


public slots:
    virtual void CmApply();
    virtual void CmBackground();

};

#endif // LEGENDDIALOGDATA_H
