/****************************************************************************
** Form interface generated from reading ui file 'SettingsDialog.ui'
**
** Created: Втр Лют 5 12:33:44 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SETTINGSDIALOGDATA_H
#define SETTINGSDIALOGDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;

class SettingsDialogData : public QDialog
{ 
    Q_OBJECT

public:
    SettingsDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SettingsDialogData();

    QPushButton* pButtonOK;
    QPushButton* pButtonApply;
    QPushButton* pButtonHelp;
    QPushButton* pButtonCancel;
    QLabel* TextLabel1_3;
    QLineEdit* pFontRawName;
    QPushButton* pButtonChFont;
    QButtonGroup* ButtonGroup1;
    QRadioButton* rbNewPrMode;
    QRadioButton* rbOldPrMode;
    QGroupBox* GroupBox1;
    QLabel* TextLabel1_2_2;
    QLabel* TextLabel1_2;
    QLabel* TextLabel1;
    QSpinBox* pUpdateInterval;
    QSpinBox* pNumDigits;
    QLabel* TextLabel1_4;
    QCheckBox* pConfigAutosave;
    QLabel* TextLabel2;
    QLabel* pUserDBDir;
    QLabel* pSysDBDir;
    QLabel* TextLabel2_2;


protected slots:
    virtual void CmChangeFont();
    virtual void CmSysDBDirSelect();
    virtual void CmUserDBDirSelect();
    virtual void CmApply();
    virtual void CmHelp();

protected:
    QHBoxLayout* Layout28;
    QHBoxLayout* Layout5;
    QGridLayout* Layout4;
};

#endif // SETTINGSDIALOGDATA_H
