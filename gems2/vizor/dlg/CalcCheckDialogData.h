/****************************************************************************
** Form interface generated from reading ui file 'CalcCheckDialog.ui'
**
** Created: Do 28. Feb 13:33:33 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CALCCHECKDIALOGDATA_H
#define CALCCHECKDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;

class CalcCheckDialogData : public QDialog
{ 
    Q_OBJECT

public:
    CalcCheckDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~CalcCheckDialogData();

    QComboBox* pValue;
    QLabel* TextLabel2;
    QGroupBox* pObjectBox;
    QLabel* TextLabel1_2;
    QLabel* TextLabel1;
    QLabel* TextLabel1_2_2;
    QLabel* pM;
    QSpinBox* pN2;
    QSpinBox* pN1;
    QPushButton* PushButton3;
    QPushButton* PushButton1_2;
    QPushButton* PushButton1;


public slots:
    virtual void CmSelectAll();
    virtual void CmOk();

protected:
};

#endif // CALCCHECKDIALOGDATA_H
