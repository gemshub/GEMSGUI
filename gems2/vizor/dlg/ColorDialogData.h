/****************************************************************************
** Form interface generated from reading ui file 'ColorDialog.ui'
**
** Created: Do 28. Feb 13:33:34 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef COLORDIALOGDATA_H
#define COLORDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QLabel;
class QPushButton;
class QRadioButton;
class QSpinBox;

class ColorDialogData : public QDialog
{ 
    Q_OBJECT

public:
    ColorDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ColorDialogData();

    QButtonGroup* pGroup;
    QRadioButton* p6;
    QRadioButton* p1;
    QRadioButton* p7;
    QRadioButton* p4;
    QRadioButton* p2;
    QRadioButton* p9;
    QRadioButton* p3;
    QRadioButton* p5;
    QRadioButton* p0;
    QRadioButton* p8;
    QLabel* TextLabel1;
    QSpinBox* pLine;
    QLabel* TextLabel2;
    QSpinBox* pSize;
    QFrame* pColor;
    QPushButton* PushButton12;
    QPushButton* PushButton10;
    QPushButton* PushButton11;


protected slots:
    virtual void CmSelectColor();

protected:
    QHBoxLayout* hbox1;
    QHBoxLayout* Layout6;
    QVBoxLayout* Layout7;
};

#endif // COLORDIALOGDATA_H
