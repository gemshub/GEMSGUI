/****************************************************************************
** Form interface generated from reading ui file 'CalcDialog.ui'
**
** Created: Fri Nov 16 14:53:54 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CALCDIALOGDATA_H
#define CALCDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class CalcDialogData : public QDialog
{ 
    Q_OBJECT

public:
    CalcDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~CalcDialogData();

    QLabel* TextLabel1_3;
    QButtonGroup* pButtonBox;
    QPushButton* PushButton3;
    QPushButton* PushButton4;
    QPushButton* PushButton5;
    QPushButton* PushButton6;
    QPushButton* PushButton7;
    QPushButton* PushButton8;
    QPushButton* PushButton9;
    QPushButton* PushButton10;
    QPushButton* PushButton12;
    QPushButton* PushButton13;
    QPushButton* PushButton15;
    QPushButton* PushButton14;
    QPushButton* PushButton11;
    QPushButton* pAssignBtn;
    QPushButton* PushButton2;
    QPushButton* pClearBtn;
    QLineEdit* pValue;
    QLabel* TextLabel1_4;
    QGroupBox* pObjectBox;
    QPushButton* PushButton22;
    QSpinBox* pM2;
    QLabel* TextLabel1_2_3;
    QSpinBox* pN2;
    QSpinBox* pN1;
    QLabel* TextLabel1_2;
    QSpinBox* pM1;
    QLabel* TextLabel1;
    QLabel* TextLabel1_2_2;
    QPushButton* PushButton1_3;
    QPushButton* PushButton1_2;
    QPushButton* PushButton42;


protected slots:
    virtual void binaryMode();
    virtual void ok();
    virtual void selectAll();
    virtual void unaryMode();

protected:
};

#endif // CALCDIALOGDATA_H
