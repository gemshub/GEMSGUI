/****************************************************************************
** Form interface generated from reading ui file 'KeyProfile.ui'
**
** Created: Mo 4. Mrz 11:35:30 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KEYPROFILEDATA_H
#define KEYPROFILEDATA_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;
class QRadioButton;

class KeyProfileData : public QDialog
{ 
    Q_OBJECT

public:
    KeyProfileData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KeyProfileData();

    QLabel* pLabel;
    QListBox* pList;
    QCheckBox* pAqGas;
    QCheckBox* pFiles;
    QCheckBox* pTemplate;
    QButtonGroup* ButtonGroup1;
    QRadioButton* rbNewPrMode;
    QRadioButton* rbOldPrMode;
    QPushButton* pGO_OKButton;
    QPushButton* pNewProfBtn;
    QPushButton* pHelpButton;
    QPushButton* pCancelButton;


public slots:
    virtual void CmNew();
    virtual void CmHelp();

protected:
    QVBoxLayout* Layout4;
    QHBoxLayout* Layout3;
};

#endif // KEYPROFILEDATA_H
