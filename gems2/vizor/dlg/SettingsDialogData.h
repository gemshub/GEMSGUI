/****************************************************************************
** Form interface generated from reading ui file 'SettingsDialog.ui'
**
** Created: Thu Nov 8 08:45:15 2001
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
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class SettingsDialogData : public QDialog
{ 
    Q_OBJECT

public:
    SettingsDialogData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SettingsDialogData();

    QLabel* TextLabel1_3;
    QLineEdit* pFontRawName;
    QPushButton* pButtonChFont;
    QPushButton* pButtonOK;
    QPushButton* pButtonApply;
    QPushButton* pButtonHelp;
    QPushButton* pButtonCancel;
    QPushButton* pButtonBrwDB;
    QLineEdit* pUserDBDir;
    QLabel* TextLabel2;
    QLabel* TextLabel2_2;
    QPushButton* pButtonBrwProf;
    QLineEdit* pDBDir;
    QLabel* TextLabel1_4;
    QSpinBox* pUpdateInterval;
    QLabel* TextLabel1;
    QLabel* TextLabel1_2;
    QSpinBox* pNumDigits;
    QLabel* TextLabel1_2_2;


protected slots:
    virtual void CmChangeFont();
    virtual void CmSysDBDirSelect();
    virtual void CmUserDBDirSelect();
    virtual void CmApply();
    virtual void CmHelp();

protected:
    QHBoxLayout* Layout5;
    QHBoxLayout* Layout28;
    QGridLayout* vbox1;
    QGridLayout* Layout5_2;
};

#endif // SETTINGSDIALOGDATA_H
